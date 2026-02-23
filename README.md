# FPGA Placement (Legalization + Detailed Placement)

這個小專案示範一個 **FPGA placement** 的簡化流程：  
1) 讀入 `architecture / instance / netlist` 三個文字檔  
2) **Legalization**：把 instance 對齊到合法的 resource 位置（同 type，盡量最小位移）  
3) **Detailed Placement**：在局部範圍內嘗試搬移或 swap，以降低總 **HPWL (Half-Perimeter Wirelength)**  
4) 輸出每個 instance 對應到的 resource 名稱

---

## 流程示意圖

### Legalization
![Legalization](./images/legalization.png)

### Detailed Placement
![Detailed Placement](./images/detailed_placement.png)
  
> - Legalization：把非法 / 浮點位置的 instance **move** 到最近的合法 resource  
> - Detailed placement：在小範圍內做 **move / swap** 來改善 HPWL

---

## Build

需要 C++17（因為有用到 `<filesystem>`）。

### g++ (Linux / WSL)
```bash
g++ -std=c++17 -O2 -o placer main.cpp
```

> 目前程式是 header-only 的結構（`main.cpp` 直接 include `*.h`），所以只要編 `main.cpp` 即可。

---

## Run

`main.cpp` 需要 4 個參數：

```bash
./placer <architecture.txt> <instance.txt> <netlist.txt> <output.txt>
```

程式內的順序（也就是跑的演算法順序）：
- `FileParser::initializeResoures`
- `FileParser::initializeInstances`
- `FileParser::initializeNets`
- `PlacementAlgorithm::legalizeByMinimunDisplacement`
- `PlacementAlgorithm::computeWirelength`（印出 HPWL）
- `PlacementAlgorithm::detailedPlacement`
- `PlacementAlgorithm::computeWirelength`（再印一次）
- `FileParser::writeOutputFile`

---

## Input format

### 1) Architecture (`architecture.txt`)
每行一個 resource：

```
<resource_name> <type> <x> <y>
```

範例：
```
RESOURCE1 CLB  1.5 0.5
RESOURCE2 RAM  2.5 1.5
RESOURCE3 DSP  3.5 2.5
```

程式會建立：
- `resourceTable[type][name]` 方便依 type 找 resource
- `resourceMap[y][x]`（用 `ceil(maxX/maxY)` 做 2D map，並把 `(int)x,(int)y` 當索引）

> 註：座標是 `double`，但 `resourceMap` 會用 `(int)` 取整當作格點索引。

---

### 2) Instance (`instance.txt`)
每行一個 instance：

```
<instance_name> <type> <x> <y>
```

範例：
```
CLB106 CLB 106.33 57.28
RAM12  RAM  12.10 90.55
```

---

### 3) Netlist (`netlist.txt`)
每行一條 net：

```
<net_name> <inst1> <inst2> <inst3> ...
```

範例：
```
net_3333 CLB107 CLB4 CLB57 CLB59 CLB8
```

程式會把 net 掛到每個 instance 的 `nets` 內（`Instance::addNet`），方便後續計算 HPWL。

> 注意：如果某行 instance 數量 < 2（`vector.size() < 3`，含 net name），程式會跳過該 net。

---

## Output format

`output.txt` 每行輸出一個 instance 的 mapping：

```
<instance_name> <mapped_resource_name>
```

- `type == "IO"` 的 instance 會被跳過（不輸出）

---

## Algorithms (對應程式碼)

### Legalization: `legalizeByMinimunDisplacement`
對每個 instance（跳過 IO）：
1. 在同 type 的可用 resources 裡，找 Manhattan distance 最小的 resource  
2. 把 instance 位置直接更新成 resource 的座標  
3. 把 resource 標記成已佔用、綁定 instance，並從 `resourceTable[type]` 移除（避免重複使用）

---

### HPWL
- `computeHPWL(placement, inst)`：計算「inst 所有 nets」的 HPWL 和  
- `computeHPWL(placement, inst, x, y)`：假設 inst 搬到 `(x,y)` 後的 HPWL（用於評估 gain）

---

### Detailed Placement: `detailedPlacement`
對每個 instance（跳過 IO）：
1. 在 `expandableRange = 2` 的局部視窗內掃描候選格點  
2. 若候選 resource 同 type：
   - 空格：評估 move gain（HPWL 降低量）
   - 非空：評估 swap gain（inst 與對方交換後，兩者 HPWL 的總 gain）
3. 若找到 gain 最大且 > 0 的候選，執行 move 或 swap

此外有時間上限：
- `timeLimit = 580000 ms`（約 9 分 40 秒），超過就停止迭代。

---

## Project structure

- `main.cpp`：整體流程（parse → legalization → detailed placement → output）
- `FilleParser.h`：讀三種檔案 + 輸出結果（檔名目前是 `FilleParser`）
- `Placement.h`：儲存 instances/resources/nets + 查表/2D map
- `Resource.h`：resource 物件（type/座標/是否佔用/佔用的 instance）
- `Instance.h`：instance 物件（type/座標/所屬 nets/映射到的 resource 名）
- `Net.h`：net 物件（net 名稱 + instance 名稱列表）
- `PlacementAlgorithm.h`：legalization / HPWL / detailed placement


