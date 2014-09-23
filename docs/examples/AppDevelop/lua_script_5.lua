
function run()
	f = provider.filter( {empty=false} )
	-- filter out tokens containing only spaces:
	input:as( f)
	output:as( f)
	t = input:value()

	-- print the document from the table
	output:opentag( 'name')
	output:print( t['name'])
	output:closetag()
	output:opentag( 'country')
	output:print( t['country'])
	output:closetag()
end

