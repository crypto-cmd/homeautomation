import { defineStore } from "pinia";
import { ref } from "vue";
import { useToast } from "vue-toastify";

export const useAppStore = defineStore(
  "app",
  () => {
    /*
    The composition API way of defining a Pinia store
    ref() s become state properties
    computed() s become getters
    function() s become actions
    */

    const setPasscode = async (passcode) => {
      const toast_status = {
        title: "Passcode Update Request",
        body: "Your passcode is atempted to update",
        mode: "loader",
      };

      const toast_id = useToast().loader(toast_status);

      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      // console.log(passcode);
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => {
        controller.abort();
      }, 60000);

      const URL = `/api/set/combination`;
      try {
        const response = await fetch(URL, {
          method: "POST",
          signal: signal,
          headers: {
            "Content-Type": "application/json",
            "User-Agent": "insomnia/9.3.3",
          },
          body: `{"code":${passcode}}`,
        });
        useToast().stopLoader(toast_id);

        if (response.ok) {
          const data = await response.json();
          let keys = Object.keys(data);
          if (keys.includes("status")) {
            if (data["status"] == "complete") {
              useToast().success({
                title: "Passcode Updated",
                body: "Your passcode was updated to " + passcode,
                canTimeout: true,
                duration: 3000,
              });
              console.log(data["data"]);
              return data["data"];
            }
            if (data["status"] == "failed") {
              console.log("setPasscode returned no data");
              useToast().error({
                title: "Passcode Update Failed",
                body: "Your passcode was not updated: setPasscode returned no data",
                canTimeout: true,
                duration: 3000,
              });
            }
          }
        } else {
          const data = await response.text();
          console.log(data);
          useToast().error({
            title: "Passcode Update Failed",
            body: "Your passcode was not updated: " + data,
            canTimeout: true,
            duration: 3000,
          });
        }
      } catch (err) {
        console.error("setPasscode error: ", err.message);

        useToast().error({
          title: "Passcode Update Failed",
          body: "Your passcode was not updated: " + err.message,
          canTimeout: true,
          duration: 3000,
        });
      }

      return [];
    };

    const update_Card = async (start, end) => {
      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      const controller = new AbortController();
      const signal = controller.signal;
      setTimeout(() => {
        controller.abort();
      }, 60000);
      const URL = `/api/avg/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          let keys = Object.keys(data);
          if (keys.includes("status")) {
            if (data["status"] == "success") {
              console.log(data["data"]);
              return data["data"];
            }
            if (data["status"] == "failed") {
              console.log("update_Card returned no data");
            }
          }
        } else {
          const data = await response.text();
          console.log(data);
        }
      } catch (err) {
        console.error("update_Card error:", err.message);
      }
      return [];
    };

    const getReserve = async (start, end) => {
      // FETCH REQUEST WILL TIMEOUT AFTER 20 SECONDS
      const controller = new AbortController();
      const signal = controller.signal;
      const id = setTimeout(() => {
        controller.abort();
      }, 60000);
      const URL = `/api/reserve/${start}/${end}`;
      try {
        const response = await fetch(URL, { method: "GET", signal: signal });
        if (response.ok) {
          const data = await response.json();
          let keys = Object.keys(data);
          if (keys.includes("status")) {
            if (data["status"] == "success") {
              console.log(data["data"]);
              return data["data"];
            }
            if (data["status"] == "failed") {
              console.log("getReserve returned no data");
            }
          }
        } else {
          const data = await response.text();
          console.log(data);
        }
      } catch (err) {
        console.error("getReserve error:", err.message);
      }
      return [];
    };
    // STATES

    // ACTIONS

    return {
      // EXPORTS
      setPasscode,
      update_Card,
      getReserve,
    };
  },
  { persist: true }
);
