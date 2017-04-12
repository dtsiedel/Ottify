import requests
import lxml
from bs4 import BeautifulSoup
import sys
import shutil


#TODO: find a way to stick this shit all together
#      so you can get the image from wikipedia       

arg = sys.argv[1]
capital = False
for ch in arg: #title case input
    if capital:
        ch = ch.upper()
        capital = False
    if ch == "_":
        capital = True

print arg

url = "https://en.wikipedia.org/wiki/{}".format(arg)
r = requests.get(url).text

soup = BeautifulSoup(r, "lxml")
image = soup.findAll("a", {"class": "image"})[0] #bit of a hack lol
image = image.img

url = "https://upload.wikimedia.org/wikipedia/commons/2/2a/" + str(image["alt"]).replace(" ", "_")
print "Getting", url
response = requests.get(url, stream=True)
with open("inputs/" + arg + ".jpg", 'wb') as out_file:
    shutil.copyfileobj(response.raw, out_file)
