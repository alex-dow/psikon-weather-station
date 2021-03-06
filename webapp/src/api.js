export const url = (!isProduction)? 'http://192.168.1.230/api' : '/api';

async function sleep(amt) {
  return new Promise((resolve) => {
    setTimeout(resolve, amt);
  });
}

export async function getStatus() {
  const response = await fetch(url + '/status');
  const data = await response.json();
  return data;
}

export async function getAvailableWifiNetworks() {
  let data = [];
  let running = true;
  let retries = 0;
  let max_retries = 10;
  while (running) {

    try {
      const response = await fetch(url + '/available-networks');

      if (response.status == 202 || response.status == 204) {
        await sleep(5000);
      } else {
        console.log('Do you ever get here???');
        data = await response.json();
        running = false;
      }
    } catch (err) {
      console.error(err);
      await sleep(5000);
    }

    retries++;

    if (retries >= max_retries) {
      running = false;
    }
  }
  console.log('so why not here?');
  return data;
}

export async function getWifiSettings() {
  const response = await fetch(url + '/wifi-settings');

  const data = await response.json();
  return data;
}

export async function saveWifiSettings(wifiSettings) {
  const formData = new URLSearchParams(wifiSettings);

  const response = await fetch(url + '/wifi-settings', {
    method: 'POST',
    body: formData,
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'
    }
  });
}

export async function getMqttSettings() {
  const response = await fetch(url + '/mqtt-settings');
  const data = await response.json();
  return data;
}

export async function saveMqttSettings(mqttSettings) {
  mqttSettings['port'] = parseInt(mqttSettings['port']);
  const formData = new URLSearchParams(mqttSettings);

  const response = await fetch(url + '/mqtt-settings', {
    method: 'POST',
    body: formData,
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8'
    }
  });
}

export async function restart() {
  await fetch(url + '/restart', {
    method: 'POST'
  });
}

export async function reset() {
  await fetch(url + '/reset', {
    method: 'POSt'
  });
}