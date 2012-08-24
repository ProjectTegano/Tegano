**
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<aa>1</aa><cc>3</cc><bb>2</bb>
**config
run(: /aa, /bb, /cc); doc=exec(/aa: $1,$2,$3)
**file: 1.result
id name street
1 hugo "bahnhof strasse 15"
2 miriam "zum gems weg 3"
3 sara "tannen steig 12"
**file: 2.result
offen
1
**file: 3.result
offen
2
**output
begin();
start( 'run' );
bind( 1, '1' );
bind( 2, '2' );
bind( 3, '3' );
execute();
nofColumns(); returns 3
get( 1 ); returns 1
get( 2 ); returns hugo
get( 3 ); returns bahnhof strasse 15
next(); returns 1
get( 1 ); returns 2
get( 2 ); returns miriam
get( 3 ); returns zum gems weg 3
next(); returns 1
get( 1 ); returns 3
get( 2 ); returns sara
get( 3 ); returns tannen steig 12
next(); returns 0
getLastError(); returns 0
start( 'exec' );
bind( 1, '1' );
bind( 2, 'hugo' );
bind( 3, 'bahnhof strasse 15' );
execute();
nofColumns(); returns 1
get( 1 ); returns 1
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns offen
bind( 1, '2' );
bind( 2, 'miriam' );
bind( 3, 'zum gems weg 3' );
execute();
nofColumns(); returns 1
get( 1 ); returns 2
next(); returns 0
getLastError(); returns 0
nofColumns(); returns 1
columnName( 1 ); returns offen
bind( 1, '3' );
bind( 2, 'sara' );
bind( 3, 'tannen steig 12' );
execute();
nofColumns(); returns 0
getLastError(); returns 0
nofColumns(); returns 0
commit();

doc {
  offen {
    '1'
  }
}
doc {
  offen {
    '2'
  }
}
**end