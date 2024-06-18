from os import environ as os_env
from sys import stdout
from datetime import datetime
from smtplib import SMTP

class CONFIG:
	def __init__(self, env: dict[str, str]):
		self.SERVER = env["EMAIL_SERVER"]
		self.SERVER_PORT = int(env["EMAIL_SERVER_PORT"])
		self.TO = env["EMAIL_TO"]
		self.NAME = env["EMAIL_NAME"]
		self.SENDER = env["EMAIL_SENDER"]
		self.PASSWORD = env["EMAIL_PASSWORD"]

CONFIG = CONFIG(os_env)

subj = os_env.get("EMAIL_SUBJ", "Email Automático")
message_text = os_env["EMAIL_TEXT"]

from_addr = f"{CONFIG.NAME} <{CONFIG.SENDER}>"
to_addr = CONFIG.TO
date = datetime.now().strftime( "%d/%m/%Y %H:%M" )

msg = f"""\
From: {from_addr}
To: {to_addr}
Subject: {subj}
Date: {date}
Content-Type: text/html

{message_text}""".encode('ascii', 'xmlcharrefreplace')

print(f"[sendmail.py]> sending email from <{CONFIG.SENDER}> to <{CONFIG.TO}>")
smtp = SMTP()
smtp.connect(CONFIG.SERVER, CONFIG.SERVER_PORT)
#smtp.starttls()
smtp.login(CONFIG.SENDER, CONFIG.PASSWORD)
smtp.sendmail(from_addr, [to_addr], msg)
print("[sendmail.py]> email sent")
smtp.quit()
