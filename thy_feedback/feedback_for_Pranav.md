1/. Consider using more meaningful names instead of twos/threes/...
``` 
splits = []
twos = []
threes = []
fours = []
fives = []
sixes = []
for item in arr:
  x = item.split("-")
  if len(x) == 2:
    twos.append(item)
    #splits.append(x[0])
  elif(len(x) == 3):
    threes.append(item)
    #splits.append(x[1] + '-' + x[2])
  elif(len(x) == 4):
    fours.append(item)
    #splits.append()
  elif(len(x) == 5):
    fives.append(item)
  else:
    sixes.append(item)
```
