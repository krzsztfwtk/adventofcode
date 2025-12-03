file = open("day2.txt", "r")

# Divide file to parts separated by commas
parts = file.read().split(",")

# Dive each part to numbers separated by hyphens
ranges = [part.split("-") for part in parts]

# For each range, interate from start to end
numbers = []
for r in ranges:
    start = int(r[0])
    end = int(r[1])
    for n in range(start, end + 1):
        numbers.append(n)

# Sum numbers which are made of sequence of numbers repeated twice
sum = 0
for n in numbers:
    str_n = str(n)
    length = len(str_n)
    if length % 2 == 0:
        half = length // 2
        if str_n[:half] == str_n[half:]:
            sum += n

# Sum numbers which are made of sequence of numbers repeated AT LEAST twice
sum2 = 0
for n in numbers:
    str_n = str(n)
    length = len(str_n)
    for i in range(1, length // 2 + 1):
        if length % i == 0:
            times = length // i
            if str_n[:i] * times == str_n:
                sum2 += n
                break

print(sum)
print(sum2)