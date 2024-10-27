# 3D-IC-placement-and-routing

### How to use
```
make
./a.out case1.txt out.txt or
./a.out case2.txt out.txt or
./a.out case3.txt out.txt or 
./a.out case4.txt out.txt
```

```
->coarsening
->initial placement(random 給一個x, y, z 座標)
->CG (x, y, z 方向的 placement) (還有問題，penalty的部分需優化)
->legalization
->write file

```
### 近期完成
- [x] 寫好placement and routing 註解
- [x] 寫好 coarsen 的註解
- [x] 利用 python 寫完 visualization
- [x] 寫完初步的 legalization
 
### Task list

- [ ] 優化 penalty 的計算
- [ ] 檢查 gradient 的問題
- [ ] legalization place2nearRow 會跳進無窮迴圈