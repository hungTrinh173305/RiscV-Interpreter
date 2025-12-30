# RISC-V Interpreter Test Guide

## Tóm tắt các thay đổi đã thực hiện:

### 1. ✅ Xóa debug printf statements
- Đã xóa các câu lệnh `printf` debug trong hàm `parse()` và `parse_saveload()`

### 2. ✅ Sửa parsing cho U-type (LUI)
- Tạo hàm `parse_utype()` mới để xử lý LUI instruction (chỉ có 2 operands: `rd, imm`)
- Cập nhật `step()` để gọi `parse_utype()` cho U-type instructions
- Lưu immediate value vào `insn_data->upperimm` đúng cách

### 3. ✅ Thêm support cho NOR instruction
- Thêm "nor" vào danh sách R-type operations
- Implement logic: `nor rd, rs1, rs2` = `~(rs1 | rs2)`

### 4. ✅ Sửa bug trong hashtable.c
- Thêm NULL check trong `ht_get()` để tránh crash khi bucket chưa được khởi tạo

### 5. ✅ Di chuyển reset register 0
- Di chuyển việc reset register 0 xuống cuối hàm `step()` để đảm bảo nó luôn được reset sau mỗi instruction

## Cách test code:

### Bước 1: Compile code
```bash
make clean
make riscv_interpreter
```

### Bước 2: Chạy test files

**Cách 1: Chạy từng test file**
```bash
./riscv_interpreter < test1.txt
./riscv_interpreter < test_program.txt
./riscv_interpreter < test_rtype.txt
# ... etc
```

**Cách 2: Chạy với debug mode**
```bash
./riscv_interpreter -d < test1.txt
```

**Cách 3: Chạy tất cả tests tự động**
- Trên Linux/Mac: `bash run_tests.sh`
- Trên Windows: `run_tests.bat`

## Danh sách test files:

1. **test1.txt** - Test file gốc từ project (R-type, I-type, U-type, Memory-type)
2. **test_program.txt** - Test file gốc đơn giản
3. **test_rtype.txt** - Test các R-type instructions (add, sub, and, or, xor, slt, sll, sra)
4. **test_itype.txt** - Test các I-type instructions với hex và decimal immediates
5. **test_lui.txt** - Test U-type (LUI) instruction riêng
6. **test_mem.txt** - Test memory operations (lw, lb, sw) với little-endian
7. **test_sb.txt** - Test store byte (sb) operation
8. **test_abi.txt** - Test ABI register names (zero, ra, sp, a0, s0, etc.)
9. **test_edge_cases.txt** - Test các edge cases (zero register, negative numbers, large shifts)
10. **test_comprehensive.txt** - Test tổng hợp tất cả instruction types
11. **test_nor.txt** - Test NOR instruction riêng

## Kiểm tra kết quả:

Sau khi chạy test, kiểm tra:
- ✅ Không có crash/segmentation fault
- ✅ Register values đúng với kỳ vọng
- ✅ Memory operations hoạt động đúng (little-endian)
- ✅ Register 0 (zero) luôn bằng 0
- ✅ LUI instruction tạo giá trị đúng (imm << 12)
- ✅ NOR instruction hoạt động đúng (~(rs1 | rs2))

## Lưu ý:

- Nếu có lỗi, kiểm tra output để xem instruction nào gây lỗi
- Sử dụng debug mode (`-d`) để xem từng instruction được execute
- Đảm bảo tất cả test files đều pass trước khi submit

