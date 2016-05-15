'''py_auxiliary.py - Python source designed to demonstrate''' 
'''the use of python scraper'''

import socket
import time

mavicona.push("hostname", socket.gethostname())
mavicona.push("datetime", time.strftime("%c"))
mavicona.push("guess", "28c15c0b405c1f7a107133edf5504367")
mavicona.push("sometime", "1459094800")

#with open('example_image.png', 'rb') as f:
#	data = f.read()
#	mavicona.push("image", data)

mavicona.save()
