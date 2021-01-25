# Two's  Complement
### Why use two's complement to represent negative numbers?

 - When it comes to operations such as addition or subtraction; two's complement is usually used to represent negative numbers, as such a leading 0 means positive , while a leading 1 usually means a negative. So if we were to add for instance (14 + (-14)), we should get  0.
 - **Using 5 Bit below (2 complement: C is denoted for Complement (you must add 1 to the one's compliment))**

|Digit||16|8| 4| 2|1|C|
|--|--|--|--|-- |--|--|--|
|14|=| 0| 1 | 1|1|0|
||
|||1|0|0|0|1|1's
||+|||||1|
|-14|=|1|0|0|1|0|2's
- So in the above to confirm -14, you simply add (-16 +2 = -14), if you were to add the binary numbers you should get 0  for addition.
-  2 complement can be also used for subtracting (14-(-14)), we should get 28, **when we subtract the above binary results  of 14 [01110] and -14 [10010], see below we first start with -14**.

| Digit| | 16 |8|4|2|1|C| 
|--|--|--|--|--|--|--|--|
| -14 | |1|0|0|1|0|
||
|||0|1|1|0|1|
||+|||||1||
|14|=|0|1|1|1|0|

We can now add the binary result of the compliment  to [01110] ( 14 ) to get 28.
