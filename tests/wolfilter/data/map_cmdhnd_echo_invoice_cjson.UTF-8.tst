**
**requires:CJSON
**requires:TEXTWOLF
**input
{
	"-doctype": "Invoice",
	"order": {
		"number": "INS03-62",
		"reference": "PO#6123",
		"representative": "happyHamster1",
		"terms": "45",
		"issuedate": "2013/03/21",
		"duedate": "2013/05/05",
		"description": "swimming pool supplies and accessoires"
	},
	"item": [
		{
			"name": "Bermuda TF 3 Pool Cleaner",
			"description": "Advanced Suction Pool Cleaning Robot",
			"quantity": "1",
			"discount": "0",
			"price": {
				"unit": "789.00",
				"total": "789.00",
				"tax": {
					"-description": "MWST 3.5%",
					"#text": "27.60"
				},
				"gross": "816.60"
			}
		},
		{
			"name": "Blue Ocean Pool Lounge - Luxury edition",
			"description": "Swimming Seat",
			"quantity": "2",
			"discount": "10",
			"price": {
				"unit": "79.30",
				"total": "142.70",
				"tax": {
					"-description": "MWST 3.5%",
					"#text": "5.00"
				},
				"gross": "147.70"
			}
		},
		{
			"name": "Pooltoyland Happy Duckburg Family",
			"description": "Disney Rubber Ducks",
			"quantity": "4",
			"discount": "0",
			"price": {
				"unit": "3.50",
				"total": "14.00",
				"tax": {
					"-description": "MWST 3.5%",
					"#text": "0.50"
				},
				"gross": "14.50"
			}
		},
		{
			"name": "Beach Ball 24",
			"description": "Inflatable Ball",
			"quantity": "2",
			"discount": "0",
			"price": {
				"unit": "1.95",
				"total": "3.90",
				"tax": {
					"-description": "MWST 3.5%",
					"#text": "0.15"
				},
				"gross": "4.05"
			}
		},
		{
			"name": "Drink Holder Tahiti Sunset",
			"description": "Drink holder",
			"quantity": "6",
			"discount": "0",
			"price": {
				"unit": "3.80",
				"total": "22.80",
				"tax": {
					"-description": "MWST 3.5%",
					"#text": "0.80"
				},
				"gross": "23.60"
			}
		}
	],
	"bill": {
		"price": {
			"total": "972.40",
			"tax": "34.05",
			"gross": "1006.45"
		},
		"payed": "0.00",
		"open": "1006.45"
	},
	"address": [
		{
			"-id": "supplier",
			"name": "The Consumer Company",
			"street": "Kreuzstrasse 34",
			"postalcode": "8008",
			"city": "ürich",
			"country": "Switzerland"
		},
		{
			"-id": "customer",
			"name": "The Consumer Company",
			"street": "Kreuzstrasse 34",
			"postalcode": "8008",
			"city": "ürich",
			"country": "Switzerland"
		},
		{
			"-id": "shipping",
			"name": "The Manufacturer Company",
			"street": "Bahnhofstrasse 2",
			"postalcode": "8001",
			"city": "ürich",
			"country": "Switzerland"
		}
	]
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson --module ../../src/modules/doctype/json/mod_doctype_json -c wolframe.conf echoInvoice

**file:wolframe.conf
LoadModules
{
	module ./../../src/modules/cmdbind/lua/mod_command_lua
	module ./../../src/modules/cmdbind/directmap/mod_command_directmap
	module ./../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform
	module ./../../src/modules/normalize/number/mod_normalize_number
	module ./../../src/modules/normalize/string/mod_normalize_string
	module ./../../src/modules/datatype/bcdnumber/mod_datatype_bcdnumber
}
Processor
{
	program ../wolfilter/scripts/invoice.sfrm
	program ../wolfilter/template/program/simpleform.wnmp
	program ../wolfilter/template/program/echo_invoice_table.lua
	cmdhandler
	{
		directmap
		{
			program test.dmap
			filter cjson
		}
	}
}
**file: test.dmap
COMMAND (echo Invoice) CALL echo_invoice_table RETURN Invoice;
**output
{
	"-doctype":	"Invoice",
	"order":	{
		"number":	"INS03-62",
		"reference":	"PO#6123",
		"representative":	"happyHamster1",
		"terms":	"45",
		"issuedate":	"2013/03/21",
		"duedate":	"2013/05/05",
		"description":	"swimming pool supplies and accessoires"
	},
	"item":	[{
			"name":	"Bermuda TF 3 Pool Cleaner",
			"description":	"Advanced Suction Pool Cleaning Robot",
			"quantity":	"1",
			"discount":	"0",
			"price":	{
				"unit":	"789.00",
				"total":	"789.00",
				"tax":	{
					"description":	"MWST 3.5%",
					"#text":	"27.60"
				},
				"gross":	"816.60"
			}
		}, {
			"name":	"Blue Ocean Pool Lounge - Luxury edition",
			"description":	"Swimming Seat",
			"quantity":	"2",
			"discount":	"10.00",
			"price":	{
				"unit":	"79.30",
				"total":	"142.70",
				"tax":	{
					"description":	"MWST 3.5%",
					"#text":	"5.00"
				},
				"gross":	"147.70"
			}
		}, {
			"name":	"Pooltoyland Happy Duckburg Family",
			"description":	"Disney Rubber Ducks",
			"quantity":	"4",
			"discount":	"0",
			"price":	{
				"unit":	"3.50",
				"total":	"14.00",
				"tax":	{
					"description":	"MWST 3.5%",
					"#text":	"0.50"
				},
				"gross":	"14.50"
			}
		}, {
			"name":	"Beach Ball 24",
			"description":	"Inflatable Ball",
			"quantity":	"2",
			"discount":	"0",
			"price":	{
				"unit":	"1.95",
				"total":	"3.90",
				"tax":	{
					"description":	"MWST 3.5%",
					"#text":	"0.15"
				},
				"gross":	"4.05"
			}
		}, {
			"name":	"Drink Holder Tahiti Sunset",
			"description":	"Drink holder",
			"quantity":	"6",
			"discount":	"0",
			"price":	{
				"unit":	"3.80",
				"total":	"22.80",
				"tax":	{
					"description":	"MWST 3.5%",
					"#text":	"0.80"
				},
				"gross":	"23.60"
			}
		}],
	"bill":	{
		"price":	{
			"total":	"972.40",
			"tax":	"34.05",
			"gross":	"1006.45"
		},
		"payed":	"0",
		"open":	"1006.45"
	},
	"address":	[{
			"id":	"supplier",
			"name":	"The Consumer Company",
			"street":	"Kreuzstrasse 34",
			"postalcode":	"8008",
			"city":	"ürich",
			"country":	"Switzerland"
		}, {
			"id":	"customer",
			"name":	"The Consumer Company",
			"street":	"Kreuzstrasse 34",
			"postalcode":	"8008",
			"city":	"ürich",
			"country":	"Switzerland"
		}, {
			"id":	"shipping",
			"name":	"The Manufacturer Company",
			"street":	"Bahnhofstrasse 2",
			"postalcode":	"8001",
			"city":	"ürich",
			"country":	"Switzerland"
		}]
}
**end
