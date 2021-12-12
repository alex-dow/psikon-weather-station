<script>
  export let open = false;
import Modal from "./Modal.svelte";
import { createEventDispatcher } from 'svelte';
import WiFiList from "./WiFiList.svelte";

const dispatch = createEventDispatcher();
</script>

<Modal id="wifi-modal" on:closed={() => dispatch('closed')} on:shown={() => dispatch('shown')} open={open} title="WiFi Networks">

  <WiFiList/>

</Modal>