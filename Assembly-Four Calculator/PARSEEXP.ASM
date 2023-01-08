PUBLIC          PARSEEXP
PAEX_DATA       SEGMENT
;EVAL_MAP        DW          0, 0,  EVAL_ADD, EVAL_SUB, EVAL_MUL, EVAL_DIV
;��ַ��������PROC�ĳ�ʽ�л������
ORIG_SS         DW          ?
ORIG_SP         DW          ?

ERR_INPUT_STR_MISMAT_PAR    DB  "Error: Mismatched Parenthesis", 0dh, 0ah, '$'
ERR_INPUT_STR_NO_OPERAND    DB  "Error: Missing Operand Somewhere", 0dh, 0ah, '$'
ERR_INPUT_STR_SYNTAX_ERROR  DB  "Error: Standard Syntax Error (No Operator between two operands)", 0dh, 0ah, '$'
ERR_INPUT_STR_UNEX_TOKEN    DB  "Error: Unexpected Token", 0dh, 0ah, '$'
ERR_INPUT_STR_UNKNOWN_1     DB  "Error: Unknown Error #1", 0dh, 0ah, '$'
ERR_INPUT_STR_UNKNOWN_2     DB  "Error: Unknown Error #2", 0dh, 0ah, '$'
ERR_INPUT_STR_UNKNOWN_3     DB  "Error: Unknown Error #3", 0dh, 0ah, '$'
ERR_INPUT_STR_INCOMPLETE_NUMBER     DB  "Error: Incomplete Number or Unexpected Arrangement of +/-", 0dh, 0ah, '$'
PAEX_DATA       ENDS

PAEX_STACK      SEGMENT     PRIVATE
;��ջ�ṹ
;��ջ����������ջ����ϣ�ǰ80H�ֽ���Ϊ����������Ķ�ջ������SP���ƣ��������ֽ�Ϊ��λ�����������ֵĶ�Ӧ������OP_ǰ׺�ĺ궨�壻
;��80H�ֽ���Ϊ����˫���ȸ������Ķ�ջ������BP���ƣ�����������SP�෴ ���԰˸��ֽ�Ϊ��λ��
;��ʼʱ��SP��BPָ����������FF�ֽ��������������û�����ı���ʽΪ�գ�����ڴ����������ã�������ʱ������ջ��������FFFFFFFFFFFFFFFF����MAIN�н����ж��Ƿ����
                BYTE        80H     DUP     (?)
SP_BOTTOM       EQU         $
                WORD        OP_BOTTOM_SP
                ALIGN       QWORD
BP_BOTTOM       EQU         $
                
                BYTE        8H      DUP     (0FFH)
                BYTE        80H     DUP     (?)
PAEX_STACK      ENDS

OP_LBRA         EQU         0
OP_RBRA         EQU         1
OP_ADD          EQU         2
OP_SUB          EQU         3
OP_MUL          EQU         4
OP_DIV          EQU         5
OP_BOTTOM_SP    EQU         0FFFFH
OP_BOTTOM_BP    EQU         0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH, 0FFH
PAEX_DEBUG      EQU         1

PAEX_CODE       SEGMENT
                ASSUME CS:PAEX_CODE, DS:PAEX_DATA
                


                EXTERN      INPUTDECIMAL: FAR, OUTPUTDECIMAL: FAR
PARSEEXP        PROC        FAR
;���룺DS:SI������ʽ���׵�ַ��DS:BX�����64bit���������׵�ַ
                PUSH        AX
                PUSH        CX
                PUSH        DX
                PUSH        BP
                PUSH        ES
                PUSHF
                PUSH        BX
                
                ;��ջ�κ�ջ��ָ��ı��治�����ö�ջ�ˣ�Ҫ���ڱ��������е�������
                MOV         AX, SS
                MOV         BX, Offset ORIG_SS
                MOV         CX, PAEX_DATA
                MOV         ES, CX
                MOV         ES:[BX], AX
                MOV         AX, SP
                MOV         BX, Offset ORIG_SP
                MOV         ES:[BX], AX

                CLI
                MOV         AX, PAEX_STACK
                MOV         SS, AX
                
                MOV         SP, Offset SP_BOTTOM             ;ָʾ�洢�������õĶ�ջͷָ��
                MOV         BP, Offset BP_BOTTOM             ;ָʾ�洢�����õ�ͷָ��
                STI
                
    ;����SP���������෴��BP����ѹջ�������ĺ�
    PUSHBP      MACRO       STH
                ADD         BP, TYPE STH
                MOV         [BP], STH
    ENDM
    POPBP       MACRO       STH
                MOV         STH, [BP]
                SUB         BP, TYPE STH
    ENDM
    PUSHBPFST   MACRO    
                ADD         BP, TYPE QWord
                FST         QWord Ptr SS:[BP]
    ENDM
    PUSHBPFSTP  MACRO     
                ADD         BP, TYPE QWord
                FSTP        QWord Ptr SS:[BP]
    ENDM
    POPBPFLD    MACRO       
                FLD         QWord Ptr SS:[BP]
                SUB         BP, TYPE Qword
    ENDM
    
    ;����ַ�
    OUTCHAR    MACRO        CHR
                PUSH        DX
                PUSH        AX
                MOV         DL, CHR
                MOV         AH, 02H
                INT         21H
                POP         AX
                POP         DX
    ENDM
    
                MOV         CX, 0H          ;CX�ڱ���������Ϊһ��ȫ������ָʾ�ոն�ȡ��ǰһ��TOKEN�Ƿ�Ϊһ�����֡�
                ;�����Ϳ���Ϊ�Ƿ���ڡ�1 2�������ѵ����ֵĴ��󣬺͡�+��-�����ױ�ʾ�Ӻż��Ż����������ṩ���ݡ�
                JMP         READ_TOKEN_LOOP
             
    ;����ջ������������һ����������ִ�и������㣬����ѹ����ջ����������DEBUG�����沨�����ŷ��������
    EVAL_STACKTOP:
                POP         BX      ;��ΪҪ�õ�ѹ�ڷ��ص�ַ�����AX�������ȵ������ص�ַ������BX���档
                POP         AX
                
        REPEAT  2                   ;���μ��ջ���Ƿ������֣�û�м������м��������֣���ִ��FLDѹ�븡������ջ��
                CMP         BP, BP_BOTTOM
                JBE         ERR_INPUT_NO_OPERAND
                
                POPBPFLD
        ENDM
        
                PUSH        BX      ;��֮ǰ���ݵķ��ص�ַ�ٴ�ѹ�룬֮���EVAL_ONCE���̲���ı�ջ��
                CALL        EVAL_ONCE
                ;POP         BX
                ;PUSH        BX
                RETN
                
    EVAL_ADD:
                FADD
        IF  PAEX_DEBUG
                OUTCHAR     '+'
        ENDIF
                JMP AFTER_EVAL
    EVAL_SUB:
                FSUBR
        IF  PAEX_DEBUG
                OUTCHAR     '-'
        ENDIF
                JMP AFTER_EVAL
    EVAL_MUL:
                FMUL
        IF  PAEX_DEBUG
                OUTCHAR     '*'
        ENDIF
                JMP AFTER_EVAL
    EVAL_DIV:
                FDIVR
        IF  PAEX_DEBUG
                OUTCHAR     '/'
        ENDIF
                JMP AFTER_EVAL
                
    EVAL_ONCE:
                CMP         AX, 02
                JE          EVAL_ADD
                CMP         AX, 03
                JE          EVAL_SUB
                CMP         AX, 04
                JE          EVAL_MUL
                CMP         AX, 05
                JE          EVAL_DIV
                JMP         ERR_INPUT_UNKNOWN_3
    AFTER_EVAL:
                
        IF  PAEX_DEBUG
                OUTCHAR     ' '
        ENDIF
                PUSHBPFSTP  ;�������ٴ�ѹ��ջ
                RETN
                
    READ_TOKEN_LOOP:        ;�����̡������ƶ�ָ��SI�����ÿһ���ֽڱ�ʾ�ķ��ţ���ȡ����
                MOV         DL, [SI]
                INC         SI
                
                CMP         DL, ' '
                JE          READ_TOKEN_LOOP
                CMP         DL, '('
                JE          GET_LBRA
                CMP         DL, ')'
                JE          GET_RBRA
                CMP         DL, '+'
                JE          GET_PLUSSIGN
                CMP         DL, '-'
                JE          GET_MINUSSIGN
                CMP         DL, '*'
                JE          GET_MULOP
                CMP         DL, '/'
                JE          GET_DIVOP
                CMP         DL, '.'
                JE          GET_FLOAT_SIGNED
                CMP         DL, '$'
                JE          GET_END_SIGN
                CMP         DL, '0'
                JB          ERR_INPUT_UNEX_TOKEN
                CMP         DL, '9'
                JA          ERR_INPUT_UNEX_TOKEN
                JMP         GET_FLOAT_NOSIGN
                
    GET_LBRA:               ;��⵽(ʱ��ѹ��ջ���ɡ�
                MOV         AX, OP_LBRA
                PUSH        AX
                XOR         CX, CX
                JMP         READ_TOKEN_LOOP
                
    GET_RBRA:               ;��⵽)ʱ������ִ�в�����ջ���Ĳ��������������ڵĵ����������ֱ������(��֮�󵯳������š�
    LOOP_OP_UNTIL_LBRA?:
                CMP         SP, SP_BOTTOM
                JE          ERR_INPUT_MISMAT_PAR
                MOV         BX, SP
                MOV         DX, SS:[BX]
                CMP         DX, OP_LBRA
                JE          AFTER_UNTIL_LBRA
                
                CALL        EVAL_STACKTOP
                JMP         LOOP_OP_UNTIL_LBRA?
                
    AFTER_UNTIL_LBRA:
                XOR         CX, CX
                ADD         SP, TYPE Word
                JMP         READ_TOKEN_LOOP
                
    GET_PLUSSIGN:                                       ;��⵽+��ʱ�������CX���жϵ����ǰ����ţ�����INPUTDECIMAL�����ǼӺŴ���
                OR          CX, CX
                JZ          GET_FLOAT_SIGNED            ;֮ǰû����������֣�����+�����Ŵ���
                ;���򵱼ӺŴ���
                JMP         GET_ADDOP
                
    GET_ADDOP:
                MOV         DX, OP_ADD                  ;�Ӻźͼ������ȼ���ͬ����ĳЩ�����غϡ�DXָʾ��ǰ������ǼӺŻ��Ǽ���
                JMP         START_ADDSUBOP
    GET_SUBOP:
                MOV         DX, OP_SUB
    START_ADDSUBOP:
                CMP         SP, SP_BOTTOM               ;ջ��û�����Ļ���ֱ�ӽ�+�Ž�ջ
                JE          PUSH_OPSTACK_ADDSUB
                MOV         BX, SP                      ;�����ȿ�һ��ջ�����������������ȼ�����ͬ���ȼ������������㣩����+�ţ��ȼ��㡣
                MOV         AX, SS:[BX]                 ;��Ȼ����Ϊֻ��+-*/�����Ҷ������ϣ����Բ�������(�Ļ������ȼ���ġ�
                SHR         AX, 1H
                CMP         AX, 2                          ;�ڶ����ȼ� */����Ӧ4��5
                JE          EVAL_STACKTOP_ADDSUB
                CMP         AX, 1                           ;��һ���ȼ� +-����Ӧ2��3
                JE          EVAL_STACKTOP_ADDSUB
                CMP         AX, 0                           ;������
                JE          PUSH_OPSTACK_ADDSUB
                JMP         ERR_INPUT_UNKNOWN_1         ;ѭ��ֱ�����Խ�+�Ž�ջ���ɡ�
        PUSH_OPSTACK_ADDSUB:
                PUSH        DX
                XOR         CX, CX
                JMP         READ_TOKEN_LOOP
        EVAL_STACKTOP_ADDSUB:
                CALL        EVAL_STACKTOP
                JMP         START_ADDSUBOP
        
    GET_MINUSSIGN:
                OR          CX, CX
                JZ          GET_FLOAT_SIGNED
                JMP         GET_SUBOP
                
    GET_MULOP:                                          ;�˳��ͼӼ�������ͬ��ֻ�����ȼ���ͬ���ѡ�
                MOV         DX, OP_MUL
                JMP         START_MULDIVOP
    GET_DIVOP:
                MOV         DX, OP_DIV
             
    START_MULDIVOP: 
                CMP         SP, SP_BOTTOM
                JE          PUSH_OPSTACK_MULDIV
                MOV         BX, SP
                MOV         AX, SS:[BX]
                SHR         AX, 1H
                CMP         AX, 2                          ;�ڶ����ȼ� */����Ӧ4��5
                JE          EVAL_STACKTOP_MULDIV
                CMP         AX, 1                           ;��һ���ȼ� +-����Ӧ2��3
                JE          PUSH_OPSTACK_MULDIV             ;��Ӽ���ͬ���˴�����ֱ�ӽ��˳���ѹջ��
                CMP         AX, 0                           ;������
                JE          PUSH_OPSTACK_MULDIV
                JMP         ERR_INPUT_UNKNOWN_2
        PUSH_OPSTACK_MULDIV:
                PUSH        DX
                XOR         CX, CX
                JMP         READ_TOKEN_LOOP
        EVAL_STACKTOP_MULDIV:
                CALL        EVAL_STACKTOP
                JMP         START_MULDIVOP
                
    GET_FLOAT_SIGNED:
                MOV         DX, SI                      ;����SIGNED����ôΪ�˷�ֹֻ����һ��+�Ż�.�ź���û�����ֵ����
                JMP         GET_FLOAT                   ;��Ҫ��SI����һ�ݣ�Ȼ��ȶԶ�ȡ����֮���DX��SI����SIû���߶������ж�Ϊ�������ִ���
    GET_FLOAT_NOSIGN:
                MOV         DX, SI
                DEC         DX                          ;������������ʱ����ȡ��SI���ϼ�һ����ʱ���Ƶ���DX������ƫ��һλ�ģ������û�����ŵ����ֵĻ�Ҫ��������
    GET_FLOAT:
                OR          CX, CX
                JNZ         ERR_INPUT_SYNTAX_ERROR
                
                DEC         SI                          ;ͬ�ϣ���������
                ADD         BP, TYPE Qword
               
                MOV         BX, BP
                PUSH        ES
                MOV         AX, SS
                MOV         ES, AX
                CALL        INPUTDECIMAL                ;���ݡ�������Ӧ�ĶμĴ����������������ֵĹ���
                POP         ES
                NOT         CX
                
                CMP         DX, SI
                JAE         ERR_INPUT_INCOMPLETE_NUMBER
                
        IF  PAEX_DEBUG
        
                PUSH        DS
                PUSH        SI
                PUSH        AX
                MOV         AX, SS
                MOV         DS, AX
                MOV         SI, BP
                CALL        OUTPUTDECIMAL               ;���沨�����ŷ��������
                POP         AX
                POP         SI
                POP         DS
                
                OUTCHAR     ' '
        ENDIF
                
                JMP         READ_TOKEN_LOOP
                
    GET_END_SIGN:
                XOR         DL, DL          ;��ȡ��endsign $������һ��DL��ʾ�Ƿ���ִ���
    GET_END_SIGN_LOOP:
                CMP         SP, SP_BOTTOM   ;Ҫִ�в�����ջ��������������                
                JE          END_PARSING
                MOV         BX, SP
                MOV         AX, SS:[BX]
                CMP         AX, OP_LBRA     ;�����������ţ�˵��������û�бպϡ�
                JE          ERR_INPUT_MISMAT_PAR

                CALL        EVAL_STACKTOP
                JMP         GET_END_SIGN_LOOP
                
    END_PARSING:
    IF  PAEX_DEBUG
                OUTCHAR     0DH
                OUTCHAR     0AH
    ENDIF
                CLI                             ;����ջ�μĴ�����λ
                MOV         BX, Offset ORIG_SP
                MOV         AX, ES:[BX]
                MOV         SP, AX
                MOV         BX, Offset ORIG_SS
                MOV         AX, ES:[BX]
                MOV         SS, AX
                STI
                CLD
                
                POP         BX                  ;��ʱ����POP��Ŀ���ַ
                PUSH        DS
                PUSH        ES
                MOV         AX, DS
                MOV         ES, AX
                MOV         AX, PAEX_STACK
                MOV         DS, AX
                MOV         SI, BP
                MOV         DI, BX              ;��ԭ����ջ��ջ��һ���ֽ�һ���ֽڵ�MOVSB��Ŀ���ַ����8��
                MOV         CX, 8H
                MOV         AL, 0FFH            ;�������쳣��DL��0��������0FFFFFFFFFFFFFFFF
                OR          DL, DL
                JNZ         MOV_FF
                REP         MOVSB
                JMP         AFTER_MOV
    MOV_FF:     REP         STOSB
    AFTER_MOV:
                POP         ES
                POP         DS
                
                
                
                POPF
                POP         ES
                POP         BP
                POP         DX
                POP         CX
                POP         AX
                RET
                
    ERR_INPUT_SHOW          MACRO   ERR_STR     ;�������ꡣ����ʱͳһ��DL���ó�FF����
        ERR_INPUT_&ERR_STR :
        IF  PAEX_DEBUG
                OUTCHAR     0DH
                OUTCHAR     0AH
        ENDIF
                PUSH        DS
                MOV         AX, PAEX_DATA
                MOV         DS, AX
                
                MOV         DX, Offset ERR_INPUT_STR_&ERR_STR
                MOV         AH, 09H
                INT         21H
                POP         DS
                
                MOV         DL, 0FFH        ;��ʾ���ִ���
                JMP         END_PARSING
    ENDM
    
    ERR_INPUT_SHOW          MISMAT_PAR
    ERR_INPUT_SHOW          NO_OPERAND
    ERR_INPUT_SHOW          SYNTAX_ERROR
    ERR_INPUT_SHOW          UNEX_TOKEN
    ERR_INPUT_SHOW          UNKNOWN_1
    ERR_INPUT_SHOW          UNKNOWN_2
    ERR_INPUT_SHOW          UNKNOWN_3
    ERR_INPUT_SHOW          INCOMPLETE_NUMBER
             
PARSEEXP        ENDP
PAEX_CODE       ENDS
END