import smtplib
from email.message import EmailMessage

EMAIL = 'singerutsav2501@gmail.com'
PASS = 'gscwujymbocimfky'
RECV = 'pastor.sklar@freemailonline.us'


def send_email(to, subject, message):
    try:
        # # create email
        msg = EmailMessage()
        msg['Subject'] = subject
        msg['From'] = EMAIL
        msg['To'] = to
        msg.set_content(message)

        # send email
        with smtplib.SMTP_SSL('smtp.gmail.com', 465) as smtp:
            smtp.login(EMAIL, PASS)
            smtp.send_message(msg)

        return True

    except Exception as e:
        print("Problem during send email")
        print(str(e))
    return False


if __name__ == "__main__":
    send_email(to=RECV,
               subject="Test Subject",
               message="Test Message")
