
function insertNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("insertNumber")(inp)
end

function updateNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("updateNumber")(inp)
end

function selectNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("selectNumber")(inp)
end

function deleteNumber()
	filter().empty = false
	local inp = input:table()
	local res = formfunction("selectNumber")(inp)
end

