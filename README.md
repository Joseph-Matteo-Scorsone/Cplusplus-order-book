This code is a simple orderbook in C++.

There are a few flaws in it fundamentally that I would like to point out. The first being the hierarchy with order types. We try to match orders by order type rather than just by what comes first in time.
Second, market orders we set to have a price of 0. There is no "market price" in our example so just setting it at 0 lets us know we want to just execute this order. Brash but gets the point across in an educational sense.
Third, and this is really nit-picky, one could say it is better to use an unordered map instead of a vector. I would agree with them but vectors are easier for this example.

I hope you enjoy!


YT channel: https://www.youtube.com/channel/UCfPHd_NBaYCczAh__YS7OAQ 
Personal Twitter: https://x.com/scorsone_joe
Business Twitter: https://x.com/prometheusmondo
