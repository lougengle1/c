# perf使用手册

##  record 用法
```
perf record -a --call-graph dwarf -p <pid>
```

## report 
```
perf report -i perf.data > perf.txt
```

## annotate
```
perf annotate
```