<script>
import MqttForm from "./MqttForm.svelte";
import WiFiForm from "./WiFiForm.svelte";
import { restarted } from './store.js';
import { restart } from './api';
import { Router, Link, Route, navigate} from 'svelte-navigator';

async function restartServer() {
  restarted.set(true);
  await restart();
}

async function goto(path) {
  return () => navigate('/' + path);
}

function reload() {
  window.location.reload();
}
</script>

<main>
  {#if $restarted}
  <section class="container">
    <h1>Server Restarted</h1>
    <p>
      The server has been restarted. If the wifi settings have changed, then the IP address of your server may have changed as well.
    </p>
    <button type="button" on:click={reload}>Reload</button>
  </section>
  {:else}
  <section class="container text-center">
    <h1>Psikon Weather Station</h1>
    <p>Version: 0.0.1</p>
    <hr/>
    <Router>
      <Route path="/sensor">
        <h1>Sensor</h1>
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
      </Route>
    </Router>
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