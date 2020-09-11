# comments 
```
// one line comment
/*
multi line comment
*/
```

# only top level
## import 
```
import UpperName;
import UpperName.Name.name;
```

## type
```
type Type <Type, Type> {
    fildName: Type;
}
type type <type, type> union {
    fildname: type;
}
type type <type, type> enum {
    fildname;
    fildname: type;
}
type Type <Type> = Type <Type>
```

## funcion
```
func (self Type) function(name: Type, name: Type) Type {
}
```

## cfuncion
```
cfunc function (name: Type, name: Type) Type {
}
```

## test
```
test {
}
```

# only in block
## var
```
var name: Type = <expretion>;
```

## const
```
const NameType = <expretion>;
```

## if
```
if () {
} else if () {
} else {
}
```

## switch
```
switch () {
case <val>:
    end;
default:
}
```

## loop
```
@label loop {
}
```

## do
```
@label do {
} while ()
```

## while
```
@label while () {
}
```

## for
```
@label for (var i: Type <expretion>) {
}
```

## repead
```
@label repead (<expretion>) {
}
```

## return
```
return <expretion>;
```

# operators
## arytmetic
```
+  -  *  /  %
+= -= *= /= %=
```
## logical
```
! || && < > <= >= == !=
```
## bitwize
```
|  &  ~  <<  >>
|= &= ~= <<= >>=
```
## array
```
[]
```
## casting
```
<expretion> as Int
```
# build in types
## signed number
```
Int Int8 Int16 Int32 Int64
```
## unsigned number
```
Uint Uint8 Uint16 Uint32 Uint64
```
## flouting point
```
Float Float32 Float64 Float128
```
## special
```
Bool
Str
Etc
None
Func <ReturnType, ArgsType>
```
