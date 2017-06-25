@----------------------------------------------------------------------------
@ File Name           : ser_io.S
@ Object              : Ein- Ausgabe-Funktionen der seriellen Schnittstelle
@			welche ueber den Supervisor-Mode gehen
@			
@
@ Namen :		Matr.-Nr.:
@			Matr.-Nr.:
@
@----------------------------------------------------------------------------

@ Debuginformationen
	.file	"ser_io.S"

@ Funktion
	.text
	.align	2
	.global	inits
	.type	inits,function
inits:
	swi	0x100
	bx	lr	@ R?cksprung

@ Funktion
	.text
	.align	2
	.global	putc
	.type	putc,function
putc:
	mov	r1, r0		@ Zeichen nach r1
	ldr	r0, =Zeichen	@ Zeiger holen
	str	r1, [r0]	@ Zeichen unter Zeiger ablegen
	swi 	0x200		@ 
	ldr	r1, =Zeichen	@ Zeiger holen
	ldr	r0, [r1]	@ Zeichen aus Zeiger holen
	bx 	lr

@ Funktion
	.text
	.align	2
	.global	getc
	.type	getc,function
getc:
	ldr 	r0, =Zeichen	@ Zeiger holen
	swi 	0x300		
	ldr	r0, =Zeichen	@ Zeiger holen		
	ldr 	r0, [r0]	@ empfangenes Zeichen zurueck geben
	bx 	lr
	
@ Funktion
	.text
	.align	2
	.global	puts
	.type	puts,function
puts:
	stmfd sp!,{lr}	@ Retten der Register

        mov r6, #0x00 @ Vergleichswert ('\0') Nullterminerung
        mov r2, #0    @ Counter
        mov r5, r0    @ Addresse nach r5
        
signload:
        ldrb r1, [r5, r2] @ Zeichen mit Offset holen (8bit aus 32bit)
        cmp r1, r6        @ Aktuelles Zeichen != 0x0A
        beq endloop
        
        cmp r1, #0x0A
        beq replaceLF
        beq jumpOver
   
        mov r0, r1
        bl putc
        cmp r0, #0
        beq signload
jumpOver:
        add r2,r2,#1
        b signload
        
        
endloop:

	ldmfd sp!,{pc}	@ R?cksprung

@ Funktion
	.text
	.align	2
	.global	gets
	.type	gets,function
gets:
	stmfd	sp!,{lr}	@ Retten der Register
	
// Hier k?nnte Ihr Code eingef?gt werden!

	ldmfd 	sp!,{pc}	@ R?cksprung
	
	
@ Funktion
	.text
	.align	2
	.global	replaceLF
	.type	replaceLF,function
replaceLF:
        stmfd	sp!,{lr}	@ Retten der Register
        @ CR und LF schreiben 
sendcr:
        mov r0, #0x0D
        bl putc
        cmp r0, #0
        beq sendcr
        
sendlf:
        mov r0, #0x0A
        bl putc
        cmp r0, #0
        beq sendlf
        
        ldmfd 	sp!,{pc}	@ R?cksprung
	
	.data
Zeichen:	.word 0			

.end
