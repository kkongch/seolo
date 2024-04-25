package com.c104.seolo.headquarter.company.entity;

import com.c104.seolo.global.common.BaseEntity;
import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.Id;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Getter;
import lombok.NoArgsConstructor;

import static jakarta.persistence.GenerationType.IDENTITY;

@NoArgsConstructor
@AllArgsConstructor
@Builder
@Getter
@Entity
public class Company extends BaseEntity {
    @Id
    @Column(name = "company_code", length = 10)
    private String companyCode;

    @Column(name = "company_name", length = 60)
    private String companyName;

    @Column(name = "company_logo", length = 255)
    private String companyLogo;

    @Column(name = "company_registration_num", length = 255)
    private String companyRegistrationNum;

    @Column(name = "company_accident_manage_num", length = 255)
    private String companyAccidentManageNum;
}