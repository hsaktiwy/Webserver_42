#!/usr/local/bin/python3

import cgi
import os
from http.cookies import SimpleCookie

sid = os.environ.get('SID','')
cookie = os.environ.get('HTTP_COOKIE','')
sessions_path = "sessions/"

