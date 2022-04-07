# fixed_outline_floorplan

## Build
```
cd build 
cmake ../ && cmake --build ./
```

## Run
```
cd ../bin

./hw3 ../testcase/ibm01.modified.txt ../output/result1

./hw3 ../testcase/ibm02.modified.txt ../output/result2

./hw3 ../testcase/ibm03.modified.txt ../output/result3

./hw3 ../testcase/ibm04.modified.txt ../output/result4

```

## Verify
```
cd ../verifier

./verify ../testcase/ibm01.modified.txt ../output/result1

./verify ../testcase/ibm02.modified.txt ../output/result2

./verify ../testcase/ibm03.modified.txt ../output/result3

./verify ../testcase/ibm04.modified.txt ../output/result4

```