function run()
	f = provider.filter()
	input:as( provider.filter("blob"))
	t = input:value()
	d = provider.document( t)
	d:as( f)
	output:as( d:metadata())

	for v,t in d:get()
	do
		output:print( v,t)
	end
end

