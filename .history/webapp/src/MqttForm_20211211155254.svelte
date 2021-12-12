<script>

async function onSubmit(e) {
  e.preventDefault();
  saveWifiSettings($wifi_ssid, $wifi_password);
}

onMount(async () => {
  loading = true;
  const data = await getWifiSettings();
  wifi_ssid.set(data.wifi_ssid);
  loading = false;
});

</script>

<form id="mqtt-form" on:submit={onSubmit}>
<fieldset>
  <legend>MQTT Settings</legend>
  <div class="form-group">
    <label for="mqtt_host">Hostname or IP Address</label>
    <input id="mqtt_host"/>
  </div>
  <div class="form-group">
    <label for="mqtt_port">Port number</label>
    <input id="mqtt_port" type="number" min="0" max="65536"/>
  </div>
  <div class="form-group">
    <label for="mqtt_username">Username</label>
    <input id="mqtt_username"/>
  </div>
  <div class="form-group">
    <label for="mqtt_password">Password</label>
    <input id="mqtt_password"/>
  </div>
  <div class="form-group">
    <label for="mqtt_id">ID</label>
    <input id="mqtt_id" /><br/>
  </div>
</fieldset>
</form>