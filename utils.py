
import requests

def getTemp():
    api_url = "https://computersshiksha.info/student-projects/iot/get_values.php?id=26"
    response = requests.get(api_url)
    if response.status_code == 200:
        data = response.json()
        return data["field2"]
    return None


def sendToServer(mask_status):
    if mask_status=="Mask":
        isMask="M"
    else:
        isMask="N"
    api_url = f"https://computersshiksha.info/student-projects/iot/set_values.php?id=26&field1={isMask}"
    response = requests.get(api_url)
    if response.status_code == 200:
        return True
    return False