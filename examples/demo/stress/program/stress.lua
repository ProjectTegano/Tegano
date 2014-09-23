
function insertNumber()
	local inp = input:table()
	local res = formfunction("insertNumber")(inp)
end

function updateNumber()
	local inp = input:table()
	local res = formfunction("updateNumber")(inp)
end

function selectNumber()
	local inp = input:table()
	local res = formfunction("selectNumber")(inp)
	output:print( res:table())
end

function deleteNumber()
	local inp = input:table()
	local res = formfunction("deleteNumber")(inp)
end

