package com.c104.seolo.domain.core.service.impl;

import com.c104.seolo.domain.core.dto.TokenDto;
import com.c104.seolo.domain.core.entity.Locker;
import com.c104.seolo.domain.core.entity.Token;
import com.c104.seolo.domain.core.exception.CoreTokenErrorCode;
import com.c104.seolo.domain.core.repository.TokenRepository;
import com.c104.seolo.domain.core.service.CoreTokenService;
import com.c104.seolo.domain.core.service.LockerService;
import com.c104.seolo.domain.user.entity.AppUser;
import com.c104.seolo.global.encryption.AesEncryption;
import com.c104.seolo.global.exception.CommonException;
import com.c104.seolo.global.security.jwt.entity.CCodePrincipal;
import com.c104.seolo.global.security.service.DBUserDetailService;
import jakarta.transaction.Transactional;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.crypto.SecretKey;

@Slf4j
@Service
@RequiredArgsConstructor
public class CoreTokenServiceImpl implements CoreTokenService {

    private final TokenRepository tokenRepository;
    private final LockerService lockerService;
    private final DBUserDetailService dbUserDetailService;



    @Override
    @Transactional
    public TokenDto issueCoreAuthToken(CCodePrincipal cCodePrincipal , String lockerUid) {
        AppUser appUser = dbUserDetailService.loadUserById(cCodePrincipal.getId());
        if (isTokenExistedForUserId(appUser.getId())) {
            throw new CommonException(CoreTokenErrorCode.EXISTED_TOKEN);
        }

        /*
        [토큰 발급 모듈]
        토큰의 value는 자물쇠의 고유 넘버(UID 혹은 serial No.) 으로 지정한다.
        해싱에 쓰이는 대칭키는 랜덤으로 생성해서 Locker DB에 저장한 값으로 한다.

        자물쇠는 서버에서 보낸 고유 넘버와 본인이 가진 고유 넘버가 동일하면 열린다.
        하지만 이 고유 넘버는 AES128로 암호화된 값으로 비교된다. (고유넘버를 알더라도 그 자체로 열 수 없다)

        대칭키인 자물쇠DB의 locker_encryption_key는 자물쇠 등록시에 아두이노에도 저장한다.
        아두이노는 서버로부터 받은 정보들을 복호화해야만 정보를 볼 수 있다.
        */
        Locker locker = lockerService.getLockerByUid(lockerUid);

        // Locker DB에 Base64로 저장되어 있는 대칭키를 가져온다.
        String base64encryptionKey = locker.getEncryptionKey();
        // 해당 대칭키 Base64를 복호화한다.
        SecretKey encryptionKey = AesEncryption.decodeBase64ToSecretKey(base64encryptionKey);
        // 복호화한 해당 대칭키를 이용해 자물쇠고유번호를 AES128 암호화한다.
        // 발급된 AES128 암호화된 UID도 Base64로 인코딩되어있다.
        String encryptedUid = AesEncryption.encrypt(lockerUid, encryptionKey);
        log.info("encrytionKey : {}", encryptionKey);
        log.info("encryptionUid :{}",encryptedUid);

        // 중복 검사
        if (tokenRepository.findByTokenValue(encryptedUid).isPresent()) {
            throw new CommonException(CoreTokenErrorCode.DUPLICATE_TOKEN_VALUE);
        }

        Token newToken = Token.builder()
                .tokenValue(encryptedUid)
                .locker(locker)
                .appUser(appUser)
                .build();
        log.info("newToken: {}", newToken.toString());
        tokenRepository.save(newToken);

        return TokenDto.of(newToken);
    }

    @Override
    public void deleteTokenByUserId(Long userId) {
        tokenRepository.findByAppUserId(userId).ifPresent(tokenRepository::delete);
    }

    @Override
    public boolean isTokenExistedForUserId(Long userId) {
        return tokenRepository.findByAppUserId(userId).isPresent();
    }

    @Override
    public void deleteTokenByTokenValue(String tokenValue) {
        tokenRepository.findById(tokenValue).ifPresent(tokenRepository::delete);
    }
}