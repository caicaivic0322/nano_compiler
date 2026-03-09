# 
# 简单计算器程序
# 演示 Python 的基本运算
#

def add(x, y):
    """加法"""
    return x + y

def subtract(x, y):
    """减法"""
    return x - y

def multiply(x, y):
    """乘法"""
    return x * y

def divide(x, y):
    """除法"""
    if y == 0:
        return "错误: 不能除以零!"
    return x / y

# 主程序
print("=== 简单计算器 ===")
print("选择运算:")
print("1. 加法")
print("2. 减法")
print("3. 乘法")
print("4. 除法")

# 示例运算
num1 = 10
num2 = 5

print(f"\n数字 {num1} 和 {num2} 的运算结果:")
print(f"加法: {add(num1, num2)}")
print(f"减法: {subtract(num1, num2)}")
print(f"乘法: {multiply(num1, num2)}")
print(f"除法: {divide(num1, num2)}")

# 测试除以零
print(f"\n除以零测试: {divide(num1, 0)}")
