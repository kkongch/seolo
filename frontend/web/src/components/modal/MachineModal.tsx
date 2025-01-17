import styled from 'styled-components';
import { Modal } from './Modal.tsx';
import { Button } from '../button/Button.tsx';
import * as Color from '@/config/color/Color.ts';
import { useNavigate } from 'react-router-dom';
import React, { useEffect, useState } from 'react';
import { MachineList } from '@/apis/Machine.ts';
import { Column } from 'react-table';
import { useTable } from 'react-table';
import { useQuery } from '@tanstack/react-query';
interface EquipmentModalProps {
  onClick?: (event: React.MouseEvent<HTMLDivElement, MouseEvent>) => void;
  option: number;
}
interface EquipmentType {
  machine_id: string;
  facility_id: string;
  facility_name: string;
  machine_name: string;
  machine_code: string;
  introduction_date: string;
  main_manager_id: string;
  main_manager_name: string;
  sub_manager_id: string;
  sub_manager_name: string;
}
const TableContainer = styled.div`
  width: 100%;
  height: 100%;
  overflow-y: auto; /* 세로 스크롤 */
`;
const ButtonBox = styled.div`
  width: 100%;
  height: auto;
  display: flex;
  justify-content: flex-end;
`;
const EquipmentModal = ({ onClick, option }: EquipmentModalProps) => {
  const navigate = useNavigate();

  const handleButtonClick = () => {
    navigate('/equipment', { state: { option } });
  };
  const [machineData, setMachineData] = useState<EquipmentType[]>([]);
  // 장비 데이터 불러오기
  const { data: machine } = useQuery({
    queryKey: ['machineList', option],
    queryFn: () => {
      if (option) {
        return MachineList(option);
      } else {
        return [];
      }
    },
  });
  useEffect(() => {
    if (machine) {
      setMachineData(
        machine.map((item: EquipmentType) => ({
          ...item,
          introduction_date: item.introduction_date.split('T')[0], // 'T' 전까지만 파싱
        })),
      );
    }
  }, [machine]);
  // 표 만들기

  const columns: Column<EquipmentType>[] = React.useMemo<
    Column<EquipmentType>[]
  >(
    () => [
      {
        Header: '작업장',
        accessor: 'facility_name',
      },
      {
        Header: '장비 명',
        accessor: 'machine_name',
      },
      {
        Header: '장비번호',
        accessor: 'machine_code',
      },
      {
        Header: '도입 일자',
        accessor: 'introduction_date',
      },
      {
        Header: '담당자',
        accessor: 'main_manager_name',
      },
    ],
    [],
  );
  const { getTableProps, getTableBodyProps, headerGroups, rows, prepareRow } =
    useTable({ columns, data: machineData });
  return (
    <Modal onClick={onClick}>
      <ButtonBox>
        <Button
          onClick={handleButtonClick}
          width={5}
          height={2.5}
          $backgroundColor={Color.GRAY200}
          $borderColor={Color.GRAY200}
          $borderRadius={0.75}
          $hoverBackgroundColor={Color.GRAY300}
          $hoverBorderColor={Color.GRAY300}
          fontSize={'1.2'}
          fontWeight={'bold'}
        >
          등록
        </Button>
      </ButtonBox>
      <TableContainer>
        <table
          {...getTableProps}
          style={{
            width: '100%',
            borderCollapse: 'collapse',
            marginTop: '1rem',
          }}
        >
          <thead>
            {headerGroups.map((headerGroup) => (
              <tr {...headerGroup.getHeaderGroupProps()}>
                {headerGroup.headers.map((column) => (
                  <th
                    {...column.getHeaderProps()}
                    style={{
                      borderBottom: '2px solid black',
                      paddingBottom: '1rem',
                      boxSizing: 'border-box',
                      color: Color.BLACK,
                      fontWeight: 'bold',
                      fontFamily: 'NYJGothicB',
                      fontSize: '1.5rem',
                    }}
                  >
                    {column.render('Header')}
                  </th>
                ))}
              </tr>
            ))}
          </thead>
          <tbody {...getTableBodyProps()}>
            {rows.map((row) => {
              prepareRow(row);
              return (
                <tr {...row.getRowProps()}>
                  {row.cells.map((cell) => (
                    <td
                      {...cell.getCellProps()}
                      style={{
                        padding: '1rem',
                        textAlign: 'center',
                        fontWeight: 'bold',
                        fontFamily: 'NYJGothicB',
                      }}
                    >
                      {cell.render('Cell')}
                    </td>
                  ))}
                </tr>
              );
            })}
          </tbody>
        </table>
      </TableContainer>
    </Modal>
  );
};

export default EquipmentModal;
