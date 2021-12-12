<script>
import { onMount } from 'svelte';
import { getMqttSettings, saveMqttSettings } from './api';
import Loader from './Loader.svelte';

let loading = false;

let settings = {
  mqtt_host: '',
  mqtt_port: 1883,
  mqtt_id: '',
  mqtt_username: '',
  mqtt_password: ''
}

async function onSubmit(e) {
  e.preventDefault();
  saveMqttSettings(settings);
}

onMount(async () => {
  loading = true;
  settings = await getMqttSettings();
  loading = false;
});

</script>

<form id="mqtt-form" on:submit={onSubmit}>
<fieldset>
  <legend>MQTT Settings</legend>
  {#if loading}
    <Loader/>
  {:else}
    <div class="form-group">
      <label for="mqtt_host">Hostname or IP Address</label>
      <input id="mqtt_host" bind:value={settings.mqtt_host}/>
    </div>
    <div class="form-group">
      <label for="mqtt_port">Port number</label>
      <input id="mqtt_port" type="number" min="0" max="65536" bind:value={settings.mqtt_port}/>
    </div>
    <div class="form-group">
      <label for="mqtt_username">Username</label>
      <input id="mqtt_username" bind:value={settings.mqtt_username}/>
    </div>
    <div class="form-group">
      <label for="mqtt_password">Password</label>
      <input id="mqtt_password" bind:value={settings.mqtt_password}/>
    </div>
    <div class="form-group">
      <label for="mqtt_id">ID</label>
      <input id="mqtt_id" bind:value={settings.mqtt_id}/><br/>
    </div>
  {/if}
</fieldset>
<button type="submit" disabled={loading}>{(loading) ? 'Loading' : 'Save'}</button>
</form>