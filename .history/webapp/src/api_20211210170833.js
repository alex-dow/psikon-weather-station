export const url = 'http://192.168.1.230';

export async function getAvailableWifiNetworks() {
  return new Promise(async (resolve, reject) => {
    const response = await fetch(url + '/available-networks');
    if (response.status == 202 || response.status == 204) {
      setTimeout(() => loadWifiNetworks(url), 5000);
    } else {
      let data = await response.json();
      resolve(data);
    }
  })
}

export async function getWifiSettings() {
  const response = await fetch(url + '/wifi-settings');

  const data = await response.json();
  return data;
}