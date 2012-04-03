**
**requires:DISABLED
**input
HELLO
RUN
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < c u s t o m e r > < n a m e > P � r n u   L i n n a v a l i t s u s < / n a m e > < a d d r e s s > < s t r e e t > S u u r - S e p a   1 6 < / s t r e e t > < d i s t r i c t > 8 0 0 9 8   P � � r n u < / d i s t r i c t > < c o u n t r y > 3 7 2 < / c o u n t r y > < / a d d r e s s > < / c u s t o m e r > 
.
QUIT
**file:input.frm

customer
{
        name STRING                        ; Name of the customer
        id @UINT                           ; Internal customer id
        address                            ; Address of the customer
        {
                street STRING              ; Street and house number
                district STRING            ; Postalcode/city/state
                country USHORT(49)         ; Countrycode, default Switzerland
        }
        link[]                             ; List of links to other customers
        {
                customerid UINT            ; Id of the linked customer
                description STRING         ; Description of this relation
        }
}

**file:output.frm

customer
{
        name STRING                        ; Name of the customer
        id @UINT                           ; Internal customer id
        address                            ; Address of the customer
        {
                street STRING              ; Street and house number
                district STRING            ; Postalcode/city/state
                country USHORT(49)         ; Countrycode, default Switzerland
        }
        link[]                             ; List of links to other customers
        {
                customerid UINT            ; Id of the linked customer
                description STRING         ; Description of this relation
        }
}

**config
proc {
directmap {
	cmd RUN
	ddl simpleform
	filter XML:textwolf
	inputform input.frm
	outputform output.frm
	function echo
}}
**output
OK enter cmd
< ? x m l   v e r s i o n = " 1 . 0 "   e n c o d i n g = " U T F - 1 6 L E "   s t a n d a l o n e = " y e s " ? > 
 < c u s t o m e r > < n a m e > P � r n u   L i n n a v a l i t s u s < / n a m e > < a d d r e s s > < s t r e e t > S u u r - S e p a   1 6 < / s t r e e t > < d i s t r i c t > 8 0 0 9 8   P � � r n u < / d i s t r i c t > < c o u n t r y > 3 7 2 < / c o u n t r y > < / a d d r e s s > < / c u s t o m e r > 
.
OK
BYE
**end

