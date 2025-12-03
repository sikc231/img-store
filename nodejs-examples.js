// Image Storage Server - Node.js Fetch Examples
// Complete examples for all API endpoints using native fetch

// Configuration
const BASE_URL = 'http://localhost:8080';
const API_KEY = process.env.IMG_STORE_API_KEY || 'your-api-key-here';

// ============================================
// 1. HEALTH CHECK (Public)
// ============================================

async function healthCheck() {
    try {
        const response = await fetch(`${BASE_URL}/health`);
        const data = await response.json();
        console.log('Health Check:', data);
        return data;
    } catch (error) {
        console.error('Health check failed:', error);
    }
}

// ============================================
// 2. UPLOAD IMAGE (Protected - Requires API Key)
// ============================================

// Upload from file path
async function uploadImage(filePath) {
    const fs = require('fs');
    
    try {
        const imageData = fs.readFileSync(filePath);
        
        const response = await fetch(`${BASE_URL}/images`, {
            method: 'POST',
            body: imageData,
            headers: {
                'Authorization': `Bearer ${API_KEY}`
                // Alternative: 'X-API-Key': API_KEY
            }
        });
        
        const result = await response.json();
        
        if (response.ok) {
            console.log('Upload successful:', result);
            return result;
        } else {
            console.error('Upload failed:', result);
            return null;
        }
    } catch (error) {
        console.error('Upload error:', error);
    }
}

// Upload from Buffer
async function uploadImageBuffer(buffer) {
    try {
        const response = await fetch(`${BASE_URL}/images`, {
            method: 'POST',
            body: buffer,
            headers: {
                'Authorization': `Bearer ${API_KEY}`
            }
        });
        
        const result = await response.json();
        console.log('Upload result:', result);
        return result;
    } catch (error) {
        console.error('Upload error:', error);
    }
}

// Upload with X-API-Key header
async function uploadImageWithXApiKey(filePath) {
    const fs = require('fs');
    
    try {
        const imageData = fs.readFileSync(filePath);
        
        const response = await fetch(`${BASE_URL}/images`, {
            method: 'POST',
            body: imageData,
            headers: {
                'X-API-Key': API_KEY
            }
        });
        
        const result = await response.json();
        console.log('Upload result:', result);
        return result;
    } catch (error) {
        console.error('Upload error:', error);
    }
}

// ============================================
// 3. DOWNLOAD IMAGE (Public - No Auth Required)
// ============================================

// Download to file
async function downloadImage(imageId, outputPath) {
    const fs = require('fs');
    
    try {
        const response = await fetch(`${BASE_URL}/images/${imageId}`);
        
        if (!response.ok) {
            console.error(`Image not found: ${imageId}`);
            return false;
        }
        
        const buffer = await response.arrayBuffer();
        fs.writeFileSync(outputPath, Buffer.from(buffer));
        
        console.log(`Downloaded to: ${outputPath}`);
        console.log(`Content-Type: ${response.headers.get('content-type')}`);
        return true;
    } catch (error) {
        console.error('Download error:', error);
        return false;
    }
}

// Download as Buffer
async function downloadImageAsBuffer(imageId) {
    try {
        const response = await fetch(`${BASE_URL}/images/${imageId}`);
        
        if (!response.ok) {
            console.error(`Image not found: ${imageId}`);
            return null;
        }
        
        const buffer = await response.arrayBuffer();
        console.log(`Downloaded ${buffer.byteLength} bytes`);
        return Buffer.from(buffer);
    } catch (error) {
        console.error('Download error:', error);
        return null;
    }
}

// Get image info without downloading
async function getImageInfo(imageId) {
    try {
        const response = await fetch(`${BASE_URL}/images/${imageId}`, {
            method: 'HEAD'
        });
        
        if (!response.ok) {
            console.error(`Image not found: ${imageId}`);
            return null;
        }
        
        return {
            contentType: response.headers.get('content-type'),
            contentLength: response.headers.get('content-length'),
            exists: true
        };
    } catch (error) {
        console.error('Error getting image info:', error);
        return null;
    }
}

// ============================================
// 4. DELETE IMAGE (Protected - Requires API Key)
// ============================================

async function deleteImage(imageId) {
    try {
        const response = await fetch(`${BASE_URL}/images/${imageId}`, {
            method: 'DELETE',
            headers: {
                'Authorization': `Bearer ${API_KEY}`
            }
        });
        
        const result = await response.json();
        
        if (response.ok) {
            console.log('Delete successful:', result);
            return true;
        } else {
            console.error('Delete failed:', result);
            return false;
        }
    } catch (error) {
        console.error('Delete error:', error);
        return false;
    }
}

// Delete with X-API-Key header
async function deleteImageWithXApiKey(imageId) {
    try {
        const response = await fetch(`${BASE_URL}/images/${imageId}`, {
            method: 'DELETE',
            headers: {
                'X-API-Key': API_KEY
            }
        });
        
        const result = await response.json();
        console.log('Delete result:', result);
        return response.ok;
    } catch (error) {
        console.error('Delete error:', error);
        return false;
    }
}

// ============================================
// 5. COMPLETE WORKFLOW EXAMPLE
// ============================================

async function completeWorkflow() {
    console.log('=== Complete Workflow Test ===\n');
    
    // 1. Health check
    console.log('1. Checking server health...');
    await healthCheck();
    console.log('');
    
    // 2. Upload image
    console.log('2. Uploading image...');
    const uploadResult = await uploadImage('test-image.jpg');
    if (!uploadResult) return;
    
    const imageId = uploadResult.id;
    console.log(`Image ID: ${imageId}\n`);
    
    // 3. Download image
    console.log('3. Downloading image...');
    await downloadImage(imageId, 'downloaded-image.jpg');
    console.log('');
    
    // 4. Get image info
    console.log('4. Getting image info...');
    const info = await getImageInfo(imageId);
    console.log('Image info:', info);
    console.log('');
    
    // 5. Delete image
    console.log('5. Deleting image...');
    await deleteImage(imageId);
    console.log('');
    
    // 6. Verify deletion
    console.log('6. Verifying deletion...');
    const deletedInfo = await getImageInfo(imageId);
    console.log('Image exists:', deletedInfo !== null);
}

// ============================================
// 6. ERROR HANDLING EXAMPLES
// ============================================

// Upload without API key (should fail with 401)
async function uploadWithoutAuth(filePath) {
    const fs = require('fs');
    
    try {
        const imageData = fs.readFileSync(filePath);
        
        const response = await fetch(`${BASE_URL}/images`, {
            method: 'POST',
            body: imageData
            // No Authorization header
        });
        
        const result = await response.json();
        
        if (response.status === 401) {
            console.log('Expected 401 Unauthorized:', result);
        } else {
            console.log('Unexpected response:', result);
        }
    } catch (error) {
        console.error('Error:', error);
    }
}

// Handle non-existent image
async function downloadNonExistentImage() {
    try {
        const response = await fetch(`${BASE_URL}/images/nonexistent123`);
        
        if (response.status === 404) {
            console.log('Expected 404: Image not found');
        } else {
            console.log(`Unexpected status: ${response.status}`);
        }
    } catch (error) {
        console.error('Error:', error);
    }
}

// ============================================
// 7. BATCH OPERATIONS
// ============================================

// Upload multiple images
async function uploadMultipleImages(filePaths) {
    const results = [];
    
    for (const filePath of filePaths) {
        console.log(`Uploading: ${filePath}`);
        const result = await uploadImage(filePath);
        if (result) {
            results.push(result);
        }
    }
    
    console.log(`Uploaded ${results.length} images`);
    return results;
}

// Download multiple images
async function downloadMultipleImages(imageIds, outputDir) {
    const fs = require('fs');
    const path = require('path');
    
    if (!fs.existsSync(outputDir)) {
        fs.mkdirSync(outputDir, { recursive: true });
    }
    
    const results = [];
    
    for (const imageId of imageIds) {
        const outputPath = path.join(outputDir, `${imageId}.jpg`);
        const success = await downloadImage(imageId, outputPath);
        results.push({ imageId, success });
    }
    
    const successCount = results.filter(r => r.success).length;
    console.log(`Downloaded ${successCount}/${imageIds.length} images`);
    
    return results;
}

// ============================================
// 8. UTILITY FUNCTIONS
// ============================================

// Check if image exists
async function imageExists(imageId) {
    try {
        const response = await fetch(`${BASE_URL}/images/${imageId}`, {
            method: 'HEAD'
        });
        return response.ok;
    } catch (error) {
        return false;
    }
}

// Get image URL for direct use
function getImageUrl(imageId) {
    return `${BASE_URL}/images/${imageId}`;
}

// Upload from URL
async function uploadFromUrl(imageUrl) {
    try {
        // Download from URL
        const downloadResponse = await fetch(imageUrl);
        const buffer = await downloadResponse.arrayBuffer();
        
        // Upload to server
        const uploadResponse = await fetch(`${BASE_URL}/images`, {
            method: 'POST',
            body: buffer,
            headers: {
                'Authorization': `Bearer ${API_KEY}`
            }
        });
        
        const result = await uploadResponse.json();
        console.log('Uploaded from URL:', result);
        return result;
    } catch (error) {
        console.error('Upload from URL error:', error);
    }
}

// ============================================
// 9. STREAM HANDLING (Advanced)
// ============================================

// Upload with progress tracking
async function uploadImageWithProgress(filePath) {
    const fs = require('fs');
    
    try {
        const stats = fs.statSync(filePath);
        const fileSize = stats.size;
        
        console.log(`Uploading ${filePath} (${fileSize} bytes)...`);
        
        const imageData = fs.readFileSync(filePath);
        
        const response = await fetch(`${BASE_URL}/images`, {
            method: 'POST',
            body: imageData,
            headers: {
                'Authorization': `Bearer ${API_KEY}`,
                'Content-Length': fileSize.toString()
            }
        });
        
        const result = await response.json();
        console.log('Upload complete:', result);
        return result;
    } catch (error) {
        console.error('Upload error:', error);
    }
}

// ============================================
// EXPORTS
// ============================================

module.exports = {
    healthCheck,
    uploadImage,
    uploadImageBuffer,
    uploadImageWithXApiKey,
    downloadImage,
    downloadImageAsBuffer,
    getImageInfo,
    deleteImage,
    deleteImageWithXApiKey,
    completeWorkflow,
    uploadWithoutAuth,
    downloadNonExistentImage,
    uploadMultipleImages,
    downloadMultipleImages,
    imageExists,
    getImageUrl,
    uploadFromUrl,
    uploadImageWithProgress
};

// ============================================
// USAGE EXAMPLES
// ============================================

// If running as main script
if (require.main === module) {
    // Example: Run complete workflow
    completeWorkflow().catch(console.error);
    
    // Example: Individual operations
    // healthCheck();
    // uploadImage('photo.jpg').then(result => console.log(result));
    // downloadImage('abc123', 'downloaded.jpg');
    // deleteImage('abc123');
}
