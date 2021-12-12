<script>
import { mqtt_host, mqtt_port, mqtt_username, mqtt_password, mqtt_id } from './store.js';

let settings = {
  settings.mqtt_host = '',
  settings.mqtt_port = 1883,
  settings.mqtt_id = '',
  settings.mqtt_username = '',
  settings.mqtt_password = ''
}

async function onSubmit(e) {
  e.preventDefault();
  saveWifiSettings({
    mqtt_host: $mqtt_host,
    mqtt_port: $mqtt_port,
    mqtt_username: $mqtt_username,
    mqtt_password: $mqtt_password,
    mqtt_id: $mqtt_id
  });
}

onMount(async () => {
  loading = true;
  const data = await getMqttSettings();
  mqtt_host.set(data.mqtt_host);
  mqtt_port.set(data.mqtt_port);
  mqtt_username.set(data.mqtt_username);
  mqtt_password.set(data.mqtt_password);
  wifi_ssid.set(data.wifi_ssid);
  loading = false;
});

</script>

<form id="mqtt-form" on:submit={onSubmit}>
<fieldset>
  <legend>MQTT Settings</legend>
  <div class="form-group">
    <label for="mqtt_host">Hostname or IP Address</label>
    <input id="mqtt_host" bind:value={$mqtt_host}/>
  </div>
  <div class="form-group">
    <label for="mqtt_port">Port number</label>
    <input id="mqtt_port" type="number" min="0" max="65536" bind:value={$mqtt_port}/>
  </div>
  <div class="form-group">
    <label for="mqtt_username">Username</label>
    <input id="mqtt_username" bind:value={$mqtt_username}/>
  </div>
  <div class="form-group">
    <label for="mqtt_password">Password</label>
    <input id="mqtt_password" bind:value={$mqtt_password}/>
  </div>
  <div class="form-group">
    <label for="mqtt_id">ID</label>
    <input id="mqtt_id" bind:value={$mqtt_id}/><br/>
  </div>
</fieldset>
</form>