
function run()
	f = filter( "xml:textwolf")
	f.empty = false
	input:as( f)
	output:as( filter( "blob"))
	t = input:table()
	f = formfunction( "print_invoice")
	output:print( f( t))
end

