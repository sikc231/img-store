#!/usr/bin/env node

const fetch = require('node-fetch');
const https = require('https');

// Create an HTTPS agent that doesn't validate certificates (for testing)
const agent = new https.Agent({
    rejectUnauthorized: false
});

async function uploadImage() {
    const imageUrl = 'https://community.fastly.steamstatic.com/economy/image/i0CoZ81Ui0m-9KwlBY1L_18myuGuq1wfhWSaZgMttyVfPaERSR0Wqmu7LAocGJKz2lu_XsnXwtmkJjSU91dh8bjn_lDkShjjoYbh_ilk7P2jZbFjKeSKMTXAk7pJs-B5XDy_kFN14mXVz4r4JHuXOAAoXJR2E7UIt0HqwdTuYuKw5gLYjYNAxCn7hi1O8G81tG_FJxt9/330x192?allow_animated=1';
    const imageName = 'ambush-sticker-capsule';
    const baseUrl = 'http://localhost:8080';
    const apiKey = ''; // Empty since server has no auth
    
    try {
        console.log(`Downloading image from: ${imageUrl}`);
        
        // Download from URL
        const downloadResponse = await fetch(imageUrl, {
            agent: agent,
            timeout: 30000
        });
        
        if (!downloadResponse.ok) {
            console.error(`Failed to download image: ${downloadResponse.status}`);
            return false;
        }
        
        const buffer = await downloadResponse.arrayBuffer();
        console.log(`Downloaded ${buffer.byteLength} bytes`);
        
        // ISSUE: arrayBuffer needs to be converted to Buffer for node-fetch
        console.log(`Uploading to: ${baseUrl}/${imageName}.png`);
        
        const uploadResponse = await fetch(`${baseUrl}/${imageName}.png`, {
            method: 'POST',
            body: Buffer.from(buffer), // Convert ArrayBuffer to Buffer
            headers: {
                'Content-Type': 'application/octet-stream',
                ...(apiKey && { 'Authorization': `Bearer ${apiKey}` })
            },
            timeout: 60000
        });
        
        if (!uploadResponse.ok) {
            const errorText = await uploadResponse.text();
            console.error(`Upload failed (${uploadResponse.status}): ${errorText}`);
            return false;
        }
        
        const result = await uploadResponse.json();
        console.log('Upload successful:', result);
        return true;
        
    } catch (error) {
        console.error(`Error: ${error.message}`);
        return false;
    }
}

uploadImage();
