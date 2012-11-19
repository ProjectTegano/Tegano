local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("[%-()]+", " "):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
end

local function content_value( v, itr)
	if v then
		return v
	end
	for v,t in itr do
		if t then
			return nil
		end
		if v then
			return v
		end
		return nil
	end
end

local function picture_value( itr)
	local picture = {}
	for v,t in itr do
		if (t == "id" or t == "caption" or t == "info" or t == "image") then
			picture[ t] = content_value( v, itr)
		end
	end
	return picture
end

local function insert_itr( tablename, parentid, itr)
	local id = 1
	local name = nil
	local nname = nil
	local description = nil
	local picture = nil
	for v,t in itr do
		if (t == "name") then
			name = content_value( v, itr)
			nname = normalizeName( name)
		elseif (t == "description") then
			description = content_value( v, itr)
		elseif (t == "picture") then
			picture = picture_value( scope( itr))
		elseif (t == "node") then
			if name then
				id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentid=parentid, picture=picture} ):table().ID
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentid=parentid} ):table().ID
	end
	return id
end

local function insert_topnode( tablename, name, description, picture, parentid)
	local nname = normalizeName( name)
	if not parentid then
		formfunction( "add" .. tablename .. "Root")( {normalizedName=nname, name=name, description=description, picture=picture} )
		return 1
	else
		local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, description=description, parentid=parentid, picture=picture} ):table().ID
		return id
	end
end

local function insert_tree_topnode( tablename, itr)
	local parentid = nil
	local id = 1
	local name = nil
	local description = nil
	local picture = nil
	for v,t in itr do
		if (t == "parent") then
			parentid = tonumber( v)
		elseif (t == "name") then
			name = content_value( v, itr)
		elseif (t == "description") then
			description = content_value( v, itr)
		elseif (t == "picture") then
			picture = picture_value( scope( itr))
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, description, picture, parentid)
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, description, picture, parentid)
	end
end

local function get_tree( tablename, parentid)
	local t = formfunction( "selectSub" .. tablename)( {id=parentid} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, description=v.description, picture=v.picture, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if i ~= parentid and v.parent then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

local function print_tree( tree, tagname, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "tree" )
	output:opentag( "item" )
	output:print( nodeid, "id")
	output:print( "\n" .. indent .. "\t")
	output:opentag( tagname)
	output:print( tree[ nodeid ].name )
	output:closetag( )
	if tree[ nodeid ].description then
		output:print( "\n" .. indent .. "\t")
		output:opentag( "description" )
		output:print( tree[ nodeid ].description )
		output:closetag( )
	end
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree( tree, tagname, v, indent .. "\t")
		n = n + 1
	end
	if n > 0 then
		output:print( "\n" .. indent)
	end
	output:closetag( )
	output:closetag()
end

local function select_tree( tablename, tagname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), tagname, id, "")
		end
	end
end

local function select_node( tablename, elementname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			output:opentag( elementname)
			output:print( v, "id")
			local r = formfunction( "select" .. tablename)( {id=v} )
			local f = form( "Category");
			f:fill( r:get())
			output:print( f:get())
			output:closetag( )
		end
	end
end

local function edit_node( tablename, itr)
	local name = nil;
	local nname = nil;
	local description = nil;
	local picture = nil;
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizeName( name)
		elseif t == "description" then
			description = content_value( v, itr)
		elseif t == "picture" then
			picture = picture_value( scope(itr))
		end
	end
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, description=description, id=id, picture=picture} )
end

local function delete_node( tablename, itr)
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	formfunction( "delete" .. tablename)( {id=id} )
end

local function create_node( tablename, itr)
	local name = nil;
	local parentid = nil;
	local description = nil;
	local picture = nil;
	for v,t in itr do
		if t == "parent" then
			parentid = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizeName( name)
		elseif t ==  "description" then
			description = content_value( v, itr)
		elseif t ==  "picture" then
			picture = picture_value( scope(itr))
		end
	end
	insert_topnode( tablename, name, description, picture, parentid)
end

local function add_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "node" then
			insert_tree_topnode( tablename, scope( itr))
		end
	end
end

function CategoryHierarchyRequest()
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	select_tree( "Category", "category", input:get())
end

function FeatureHierarchyRequest()
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	select_tree( "Feature", "feature", input:get())
end

function pushCategoryHierarchy()
	add_tree( "Category", input:get())
end

function pushFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function CategoryRequest()
	output:as( "node SYSTEM 'Category.simpleform'")
	select_node( "Category", "category", input:get())
end

function FeatureRequest()
	output:as( "node SYSTEM 'Feature.simpleform'")
	select_node( "Feature", "feature", input:get())
end

function readCategory()
	print_tree( get_tree( "Category", 1), "category", 1, "")
end

function editCategory()
	edit_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), "category", 1, "")
end

function editFeature()
	edit_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), "feature", 1, "")
end

function deleteCategory()
	delete_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), "feature", 1, "")
end

function deleteFeature()
	delete_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), "feature", 1, "")
end

function createCategory()
	create_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), "category", 1, "")
end

function createFeature()
	create_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), "feature", 1, "")
end

