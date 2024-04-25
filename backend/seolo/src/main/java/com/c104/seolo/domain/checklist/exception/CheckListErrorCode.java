package com.c104.seolo.domain.checklist.exception;

import lombok.AllArgsConstructor;
import lombok.Getter;
import org.springframework.http.HttpStatus;

@Getter
@AllArgsConstructor
public enum CheckListErrorCode {
    NOT_EXIST_CHECK_LIST("체크리스트를 조회할 수 없습니다.", HttpStatus.BAD_REQUEST);

    private final String message;
    private final HttpStatus httpStatus;
}