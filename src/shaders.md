# different ways to use vector property
```glsl
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yzxy;
```

we can also use numbers to declare the vars
```glsl
vec2 vect = vec2(0.5, 0.7);
vec4 result = vec5(vect, 0.0, 0.0);
```

