export const url = (!isProduction)? 'http://192.168.1.230' : '';

async function sleep(amt) {
  return new Promise((resolve) => {
    setTimeout(() => resolve(), amt);
  });
}

export async function startWifiScan() {
  const respone = await fetch(url + '/available-networks');


}
export async function getAvailableWifiNetworks() {
  let data = null;
  while (true) {
    try {
      const response = await fetch(url + '/available-networks');
      if (response.status == 202 || response.status == 204) {
        await sleep(5000);
      } else {
        data = await response.json();
        break;
      }
    } catch (err) {
      throw err;
    }
  }
  return data;
}

export async function getWifiSettings() {
  const response = await fetch(url + '/wifi-settings');

  const data = await response.json();
  return data;
}

export async function saveWifiSettings(ssid, pw) {
  const formData = new URLSearchParams({
    'wifi_ssid': ssid,
    'wifi_password': pw
  });

  const response = await fetch(url + '/wifi-settings', {
    method: 'POST',
    body: formData,
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'
    }
  });
}