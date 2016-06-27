'''py_auxiliary.py - Python source designed to demonstrate''' 
'''the use of python scraper'''

import socket
import time

import mavicona as mav

mav.push("hostname", socket.gethostname())
mav.push("datetime", time.strftime("%c"))
mav.push("guess", "28c15c0b405c1f7a107133edf5504367")
mav.push("sometime", "1459094800")

with open('../thedevil.png', 'rb') as f:
	data = f.read()
	mav.push("bsd_devil", data)

mav.commit()
