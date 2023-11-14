#! /bin/python3

from sys import argv, stderr
from typing import Any
import requests
from bs4 import BeautifulSoup

argv.pop(0)
SITE = argv.pop(0)
config: dict[str, Any] = {
    "headers": {},
    "usehttp": False,
    "depth": 2,
    "path": "/",
    "cookies": {},
}

while argv:
    arg = argv.pop(0)
    if (arg in config):
        value = argv.pop(0)
        try:
            config[arg] = eval(value)
        except (NameError, SyntaxError):
            config[arg] = value

def scrape(url: str) -> set[str]:
    r = requests.get(url, cookies=config["cookies"])
    html = BeautifulSoup(r.text,"html.parser")
    links = set([tag.get("href") for tag in html.find_all('a', href=True)])
    return links

def keep_from(links: set[str], site: str) -> set[str]:
    return {link for link in links if link.startswith(site)}

links: set[str] = keep_from(scrape(SITE+config["path"]), SITE)
seen:dict[str, set[str]] = { }

while links:
    link = links.pop()
    if link in seen: continue
    newlinks = keep_from(scrape(link), SITE)
    seen[link] = newlinks
    newlinks = {newlink for newlink in newlinks if newlink not in seen}
    # add unseen links
    links.update(newlinks)
    links = {link for link in links if link not in seen}
    print(f"scanning: {link} // {len(newlinks)} to go\n", newlinks, file=stderr)

print(seen)

