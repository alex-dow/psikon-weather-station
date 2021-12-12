<script>
import MqttForm from "./MqttForm.svelte";
import WiFiForm from "./WiFiForm.svelte";
import { getStatus, restart, reset } from './api';
import { Router, Route, navigate } from 'svelte-navigator';
import { onMount } from "svelte";
import Loader from "./Loader.svelte";


let restarted = false;
let loading   = true;
let status = {
  ipAddr: '0.0.0.0',
  wifiFailed: false,
  mqttFailed: false,
  hasWifiConfig: false
}

async function restartServer() {
  restarted = true;
  await restart();
}

async function resetServer() {
  restarted = true;
  await reset();
}

function goto(path) {
  return () => {
    navigate('/' + path);
  }
}

function reload() {
  window.location.reload();
}

async function ping() {
  loading = true;
  status = await getStatus();
  loading = false;
}

onMount(() => {
  loading = true
})
</script>

<main>
  {#if restarted}
  <section class="container">
    <h1>Server Restarted</h1>
    <p>
      The server has been restarted. If the wifi settings have changed, then the IP address of your server may have changed as well.
    </p>
    <button type="button" on:click={reload}>Reload</button>
  </section>
  {:else}
  <section class="container text-center">
    <h1 on:click={goto('')}>
        Psikon Weather Station
    </h1>
    <p>Version: 0.0.1</p>

    <hr/>
    {#if loading}
      <Loader/>
    {:else}
    {#if status.wifiFailed}
      <div class="warning-container">
        The WiFi connection has failed.
      </div>
    {/if}
    {#if status.mqttFailed}
      <div class="warning-container">
        The MQTT connection has failed.
      </div>
    {/if}
    {#if !status.hasWifiConfig}
      <div class="warning-container">
        No WiFi configuration found.
      </div>
    {/if}
      <Router>
        <Route path="/sensors">
          <h1>Sensors</h1>
        </Route>
        <Route path="/wifi-form">
          <WiFiForm/>
        </Route>
        <Route path="/mqtt-form">
          <MqttForm/>
        </Route>
        <Route>
          <h3>Choose a Section:</h3>
          <button type="button" on:click={goto('sensors')}>View Sensor</button><br/>
          <button type="button" on:click={goto('wifi-form')}>WiFi Settings</button><br/>
          <button type="button" on:click={goto('mqtt-form')}>MQTT Settings</button><br/>
          <button type="button" on:click={restartServer}>Restart Server</button><br/>
          <button type="button" on:click={resetServer}>Reset All Configuration</button>
        </Route>
      </Router>
    {/if}
  </section>
  {/if}
</main>


<style>
 main {
  min-height: 100vh;
  width: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  background-color: aliceblue;
 }
 .container {
  max-width: 400px;
  width: 90%;
  padding: 20px;
  box-shadow: 0px 0px 20px #00000020;
  border-radius: 8px;
  background-color: white;
 }

</style>