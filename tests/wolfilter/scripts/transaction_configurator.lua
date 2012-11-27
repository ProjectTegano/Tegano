local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("[%-()]+", " "):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
end

local function content_value( v, itr)
	if v then
		return v
	end
	for v,t in itr do
		if t and v then
		else
			if t then
				return nil
			end
			if v then
				return v
			end
		end
	end
end

local function picture_value( itr)
	local picture = {}
	for v,t in itr do
		if (t == "id" or t == "caption" or t == "info" or t == "image") then
			picture[ t] = content_value( v, itr)
		end
	end
	info = formfunction( "imageInfo" )( { [ "data"] = picture["image"] } ):table( )
	picture["width"] = info.width
	picture["height"] = info.height
	thumb = formfunction( "imageThumb" )( { [ "image" ] = { [ "data" ] = picture["image"] }, [ "size" ] = 50 } ):table( )
	picture["thumbnail"] = thumb.data
	return picture
end

local function insert_itr( tablename, parentID, itr)
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
--DEBUG				logger.printc( "add" .. tablename, {name=name, normalizedName=nname, description=description, parentID=parentID, picture=picture} )
				id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentID=parentID, picture=picture} ):table().ID
--DEBUG				logger.printc( "ID ", id)
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
--DEBUG		logger.printc( "add" .. tablename, {name=name, normalizedName=nname, description=description, parentID=parentID, picture=picture} )
		id = formfunction( "add" .. tablename)( {name=name, normalizedName=nname, description=description, parentID=parentID, picture=picture} ):table().ID
--DEBUG		logger.printc( "ID ", id)
	end
	return id
end

local function insert_topnode( tablename, name, description, picture, parentID)
	local nname = normalizeName( name)
	if not parentID then
		parentID = 1
	end
--DEBUG	logger.printc( "add" .. tablename, {name=name, normalizedName=nname, description=description, parentID=parentID, picture=picture} )
	local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, description=description, parentID=parentID, picture=picture} ):table().ID
--DEBUG	logger.printc( "ID ", id)
	return id
end

local function insert_tree_topnode( tablename, itr)
	local parentID = nil
	local id = 1
	local name = nil
	local description = nil
	local picture = nil
	for v,t in itr do
		if (t == "parentID") then
			parentID = tonumber( v)
		elseif (t == "name") then
			name = content_value( v, itr)
		elseif (t == "description") then
			description = content_value( v, itr)
		elseif (t == "picture") then
			picture = picture_value( scope( itr))
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, description, picture, parentID)
				name = nil
				description = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, description, picture, parentID)
	end
end

local function get_tree( tablename, parentID)
	local t = formfunction( "selectSub" .. tablename)( {id=parentID} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, description=v.description, picture=v.picture, parentID=tonumber(v.parentID), children = {} } )
	end
	for i,v in pairs( a) do
		if i ~= parentID and v.parentID then
			table.insert( a[ v.parentID ].children, i )
		end
	end
	return a
end

local function print_tree( tree, tagname, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "tree" )
	if tree[ nodeid ] then
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
	end
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
			local r = formfunction( "select" .. tablename)( {id=v} )
			local f = form( tablename);
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
			-- don't allow editing of the root element (fast hack)
			if id == "1" then
				return
			end
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
	-- don't allow deletion of the root element (fast hack)
	if id == "1" then
		return
	end
--DEBUG	logger.printc( "delete" .. tablename, {id=id} )
	formfunction( "delete" .. tablename)( {id=id} )
end

local function create_node( tablename, itr)
	local name = nil;
	local parentID = nil;
	local description = nil;
	local picture = nil;
	for v,t in itr do
		if t == "parentID" then
			parentID = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizeName( name)
		elseif t ==  "description" then
			description = content_value( v, itr)
		elseif t ==  "picture" then
			picture = picture_value( scope(itr))
		end
	end
	insert_topnode( tablename, name, description, picture, parentID)
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
	output:as( "tree SYSTEM 'CategoryHierarchy.simpleform'")
	select_tree( "Category", "category", input:get())
end

function FeatureHierarchyRequest()
	output:as( "tree SYSTEM 'FeatureHierarchy.simpleform'")
	select_tree( "Feature", "feature", input:get())
end

function TagHierarchyRequest()
	output:as( "tree SYSTEM 'TagHierarchy.simpleform'")
	select_tree( "Tag", "tag", input:get())
end

function pushCategoryHierarchy()
	add_tree( "Category", input:get())
end

function pushFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function pushTagHierarchy()
	add_tree( "Tag", input:get())
end

function CategoryRequest()
	output:as( "category SYSTEM 'Category.simpleform'")
	select_node( "Category", "category", input:get())
end

function FeatureRequest()
	output:as( "feature SYSTEM 'Feature.simpleform'")
	select_node( "Feature", "feature", input:get())
end

function TagRequest()
	output:as( "tag SYSTEM 'Tag.simpleform'")
	select_node( "Tag", "tag", input:get())
end

function editCategory()
	edit_node( "Category", input:get())
end

function editFeature()
	edit_node( "Feature", input:get())
end

function editTag()
	edit_node( "Tag", input:get())
end

function deleteCategory()
	delete_node( "Category", input:get())
end

function deleteFeature()
	delete_node( "Feature", input:get())
end

function deleteTag()
	delete_node( "Tag", input:get())
end

function createCategory()
	create_node( "Category", input:get())
end

function createFeature()
	create_node( "Feature", input:get())
end

function createTag()
	create_node( "Tag", input:get())
end

function PictureListRequest()
	output:as( "list SYSTEM 'PictureList.simpleform'" )
	filter().empty = false
	local t = formfunction( "selectPictureList" )( {} ):table( )
	output:opentag( "list" )
	for k,v in pairs( t ) do
		output:opentag( "picture" )
		output:print( v )
		output:closetag( )
	end
	output:closetag( )
end


function run()
	filter().empty = false
	output:as( "result SYSTEM 'test.simpleform'")
	output:opentag("result")
	local itr = input:get()
	for v,t in itr do
		if (t == "pushCategoryHierarchy") then
			add_tree( "Category", scope(itr))
		elseif (t == "pushFeatureHierarchy") then
			add_tree( "Feature", scope(itr))
		elseif (t == "pushTagHierarchy") then
			add_tree( "Tag", scope(itr))
		elseif (t == "CategoryHierarchyRequest") then
			select_tree( "Category", "category", scope(itr))
		elseif (t == "FeatureHierarchyRequest") then
			select_tree( "Feature", "feature", scope(itr))
		elseif (t == "TagHierarchyRequest") then
			select_tree( "Tag", "tag", scope(itr))
		elseif (t == "editCategory") then
			edit_node( "Category", scope(itr))
		elseif (t == "editFeature") then
			edit_node( "Feature", scope(itr))
		elseif (t == "editTag") then
			edit_node( "Tag", scope(itr))
		elseif (t == "deleteCategory") then
			delete_node( "Category", scope(itr))
		elseif (t == "deleteFeature") then
			delete_node( "Feature", scope(itr))
		elseif (t == "deleteTag") then
			delete_node( "Tag", scope(itr))
		elseif (t == "createCategory") then
			create_node( "Category", scope(itr))
		elseif (t == "createFeature") then
			create_node( "Feature", scope(itr))
		elseif (t == "createTag") then
			create_node( "Tag", scope(itr))
		elseif (t == "CategoryRequest") then
			select_node( "Category", "category", scope(itr))
		elseif (t == "FeatureRequest") then
			select_node( "Feature", "feature", scope(itr))
		elseif (t == "TagRequest") then
			select_node( "Tag", "tag", scope(itr))
		end
	end
	output:closetag()
end