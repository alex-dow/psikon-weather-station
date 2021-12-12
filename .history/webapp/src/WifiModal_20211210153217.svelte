<script>
import Modal from "./Modal.svelte";
import { createEventDispatcher } from 'svelte';

const dispatch = createEventDispatcher();
</script>

<Modal id="wifi-modal" on:close={dispatch('close')} on:shown={dispatch('shown')} >

  <p>List of wifi stuff here</p>

</Modal>