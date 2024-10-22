# 3D-IC-placement-and-routing

### How to use
```
make
./a.out case1.txt or
./a.out case2.txt or 
./a.out case3.txt or 
./a.out case4.txt 
```

```
coarsening->
initial placement(random 給一個x, y, z 座標)->
CG (x, y, z 方向的 placement)-> (還有問題，penalty的部分需優化)
legalization->
write file

```


### Task list
- [ ] 將 print 所有data structure 的function 寫出來
- [ ] 寫好placement and routing 註解
- [ ] 寫好 coarsen 的註解