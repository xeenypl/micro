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

## enum
```
enum {
    fildName;
    fildName: Type;
} name
```

## union
```
union {
    fildName: Type;
} name
```

## type
```
type Type <Type, Type> {
    fildName: Type;
}
type Type <Type, Type> enum {
    fildName;
    fildName: Type;
}
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

# top lever and block level
## var
```
var name: Type = <expretion>;
```

## const
```
const NameType = <expretion>;
```

# only in block
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
@label for (let i = 1; i < 10; i += 1) {
}
```

## foreatch
```
@label foreatch (x: <expretion>) {
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
ref Int <expretion>
```
