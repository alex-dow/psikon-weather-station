<script>
import MqttForm from "./MqttForm.svelte";
import WiFiForm from "./WiFiForm.svelte";
import { restarted } from './store.js';
import { restart } from './api';

async function onRestart() {
  restarted.set(true);
  await restart();
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
    <button type="button" on:click={onRestart}>Restart Server</button>
    <form>
      <WiFiForm/>
      <MqttForm/>
    </form>
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