RED         EQU 04h
GREEN       EQU 02h
LIGHTRED    EQU 0Ch
LIGHTGREEN  EQU 0Ah

DATASG  SEGMENT PARA 'Data'
        STRING  DB      'LUCKY'
DATASG  ENDS

CODESG	SEGMENT  PARA  'Code'
        ASSUME  CS: CODESG, DS: CODESG, SS: CODESG
        ORG     100H

MAIN	PROC	NEAR
	MOV	AH,00	      ; ��������� ������������ ������
	MOV	AL,0DH	      ;
	INT	10H

        MOV     AH,0Bh
        MOV     BH,00
        MOV     BL,GREEN
        INT     10H



      ;��������� ������� � ������� 4:4
	MOV	AH,02H
	MOV	BH,00H
	MOV	DH,04H
	MOV	DL,04H
	INT	10H

	;����� ���� ��������
	MOV	AH,09H	      	;
	MOV	AL,66	      	; ��� �������
	MOV	BH,00H	      	;
        MOV     BL,GREEN        ; ����
        MOV     CX,01H          ; ���-�� ��������
	INT	10H


	;������������� ���� ��������
	MOV	BX,LIGHTRED	; ���������  ����;
	MOV	CX,32       	; �������
	MOV	DX,42       	; � ������

       @1:
	MOV	AH,0CH  	; �������  ������  �����
	MOV	AL,BL       	; ����������  ����
	INT	10H		; BX, CX  �  DX  ����������
	INC	CX		; ���������  �������
	CMP	CX,70       	; �������=70 ?
	JNA	@1		; ��� - ����

	; ����� ������� �������
	MOV	AH,00H
	INT	16H
	; ���������� ��������� �����
	MOV	AH,00	       ; ��������� ���������� ������
	MOV	AL,03H	 	;
	INT	10H
	; ����� �� ���������
	MOV	AL,0
	MOV	AH,04CH
	INT	21H
	RET

MAIN	ENDP
CODESG	ENDS
END
