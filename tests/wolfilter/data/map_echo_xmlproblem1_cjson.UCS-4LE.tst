**
**requires:CJSON
**requires:TEXTWOLF
**input
{   
   	   "   -   d   o   c   t   y   p   e   "       :       "   C   a   t   e   g   o   r   y   H   i   e   r   a   r   c   h   y   R   e   q   u   e   s   t   "   ,   
   	   "   -   i   d   "   :       "   1   "   
   }   **config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json - 
**output
{   
   	   "   -   d   o   c   t   y   p   e   "   :   	   "   C   a   t   e   g   o   r   y   H   i   e   r   a   r   c   h   y   R   e   q   u   e   s   t   "   ,   
   	   "   i   d   "   :   	   "   1   "   
   }   
   **end
